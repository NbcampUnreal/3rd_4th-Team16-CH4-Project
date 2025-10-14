// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POStageGameMode.h"
#include "game/POStageGameState.h"
#include "game/POLobbyPlayerState.h"

#include "Controllers/POPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameSession.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "CollisionQueryParams.h"
#include "HAL/CriticalSection.h"
#include "GenericTeamAgentInterface.h"
#include "OnlyOne/OnlyOne.h"

APOStageGameMode::APOStageGameMode()
{
	NumAIsToSpawn = 8;
	AIRandomRadius = 2000.0f;
	PrepSeconds = 10;
	StageSeconds = 20;
	bDidSubscribePhase = false;

	GameStateClass = APOStageGameState::StaticClass();

	bUseSeamlessTravel = true;
}

APOLobbyPlayerState* APOStageGameMode::ToLobbyPS(AController* C) const
{
	return (C && C->PlayerState) ? Cast<APOLobbyPlayerState>(C->PlayerState) : nullptr;
}

APOLobbyPlayerState* APOStageGameMode::ToLobbyPS(AActor* A) const
{
	const APawn* P = Cast<APawn>(A);
	const AController* C = P ? P->GetController() : Cast<AController>(A);
	return (C && C->PlayerState) ? Cast<APOLobbyPlayerState>(C->PlayerState) : nullptr;
}

void APOStageGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	const FString ReturnURL = UGameplayStatics::ParseOption(Options, TEXT("Return"));
	if (!ReturnURL.IsEmpty())
	{
		LobbyMapURL = ReturnURL;
		LOG_NET(POLog, Warning, TEXT("[StageGM] InitGame: LobbyMapURL set from Return option: %s"), *LobbyMapURL);
	}
	else
	{
		LOG_NET(POLog, Warning, TEXT("[StageGM] InitGame: No Return option. Using default LobbyMapURL=%s"), *LobbyMapURL);
	}

	if (GameSession)
	{
		GameSession->MaxPlayers = MaxPlayersInStage;
	}
}

void APOStageGameMode::BeginPlay()
{
	Super::BeginPlay();

	LOG_NET(
		POLog,
		Warning,
		TEXT("[StageGM] BeginPlay: PrepSeconds=%d, NumAIsToSpawn=%d, AIRadius=%.1f"),
		PrepSeconds,
		NumAIsToSpawn,
		AIRandomRadius
	);

	AlivePlayers.Reset();
	UsedPlayerStarts.Reset();
	ResetAllPlayerStatesForMatchStart();
	bActivatedWithEnoughPlayers = false; // 라운드 시작 시 플래그 리셋
	CachePlayerStarts();
	SpawnAIsOnStage();

	if (AGameStateBase* GSBase = GameState)
	{
		for (APlayerState* PSBase : GSBase->PlayerArray)
		{
			if (APOLobbyPlayerState* PS = Cast<APOLobbyPlayerState>(PSBase))
			{
				AlivePlayers.Add(PS);
				PS->SetAlive_ServerOnly(true);
			}
		}
		LOG_NET(POLog, Warning, TEXT("[StageGM] AlivePlayers initialized: %d"), AlivePlayers.Num());
	}

	if (APOStageGameState* GS = GetGameState<APOStageGameState>())
	{
		if (!bDidSubscribePhase)
		{
			GS->OnPhaseChanged.AddUObject(this, &APOStageGameMode::HandlePhaseChanged);
			GS->OnStageTimeExpired.AddUObject(this, &APOStageGameMode::HandleStageTimeExpired);
			bDidSubscribePhase = true;
		}

		LOG_NET(POLog, Warning, TEXT("[StageGM] Requesting prep countdown start (%d sec)"), PrepSeconds);
		GS->ServerStartPrepCountdown(PrepSeconds);
	}
	else
	{
		LOG_NET(POLog, Error, TEXT("[StageGM] GameState not found!"));
	}
}

void APOStageGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UsedPlayerStarts.Reset();

	if (APOStageGameState* GS = GetGameState<APOStageGameState>())
	{
		if (bDidSubscribePhase)
		{
			GS->OnPhaseChanged.RemoveAll(this);
			GS->OnStageTimeExpired.RemoveAll(this);
			bDidSubscribePhase = false;
		}
	}

	for (TActorIterator<AAIController> It(GetWorld()); It; ++It)
	{
		if (AAIController* AIC = *It)
		{
			if (AIC->BrainComponent)
			{
				AIC->BrainComponent->StopLogic(TEXT("LevelEnd"));
			}
			AIC->StopMovement();
		}
	}

	CachedPlayerStarts.Empty();

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(ReturnToLobbyTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void APOStageGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (!ErrorMessage.IsEmpty())
	{
		return;
	}

	const APOStageGameState* GS = GetGameState<APOStageGameState>();
	if (GS)
	{
		const EPOStagePhase Phase = GS->GetPhase();

		// Prep에서만 인원 제한 체크
		if (Phase == EPOStagePhase::Prep)
		{
			const int32 MaxPlayers = (GameSession && GameSession->MaxPlayers > 0) ? GameSession->MaxPlayers : MaxPlayersInStage;
			const int32 Current    = GetNumPlayers() + NumTravellingPlayers;

			if (Current >= MaxPlayers)
			{
				ErrorMessage = TEXT("Stage is full.");
				return;
			}
		}

		// Active/RoundEnd/GameEnd 중간 입장 차단
		const bool bBlockMidJoin =
			(Phase == EPOStagePhase::Active)   ||
			(Phase == EPOStagePhase::RoundEnd) ||
			(Phase == EPOStagePhase::GameEnd);

		if (bBlockMidJoin)
		{
			ErrorMessage = TEXT("Mid-join is temporarily disabled on Stage.");
			return;
		}
	}
}

void APOStageGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		if (APOLobbyPlayerState* PS = ToLobbyPS(NewPlayer))
		{
			AlivePlayers.Add(TWeakObjectPtr<APlayerState>(PS));
			PS->SetAlive_ServerOnly(true);

			LOG_NET(POLog, Log, TEXT("[StageGM] PostLogin: Add Alive %s, AliveNow=%d"),
				*PS->GetPlayerName(), AlivePlayers.Num());
		}

		RestartPlayer(NewPlayer);
		
		if (const APOStageGameState* GS = GetGameState<APOStageGameState>())
		{
			if (GS->GetPhase() == EPOStagePhase::Active)
			{
				if (CountAliveHumans() >= MinPlayersToStart)
				{
					bActivatedWithEnoughPlayers = true;
					LOG_NET(POLog, Warning, TEXT("[StageGM] Now enough players during Active. Winner decisions enabled."));
				}
			}
		}
	}
}

void APOStageGameMode::Logout(AController* Exiting)
{
	if (Exiting)
	{
		if (APOLobbyPlayerState* PS = ToLobbyPS(Exiting))
		{
			PS->ServerSetAlive(false);
			LOG_NET(POLog, Warning, TEXT("[StageGM] Logout: Mark DEAD %s (Id=%d)"),
				*PS->GetPlayerName(), PS->GetPlayerId());

			const bool bRemoved = (AlivePlayers.Remove(PS) > 0);
			LOG_NET(POLog, Log, TEXT("[StageGM] Logout: Remove Alive %s (removed=%s)"),
				*PS->GetPlayerName(), bRemoved ? TEXT("true") : TEXT("false"));

			CompactAlivePlayers();
			TryDecideWinner();
		}
	}

	Super::Logout(Exiting);
}

void APOStageGameMode::HandleSeamlessTravelPlayer(AController*& Controller)
{
	Super::HandleSeamlessTravelPlayer(Controller);

	if (Controller)
	{
		if (APOLobbyPlayerState* PS = ToLobbyPS(Controller))
		{
			if (!AlivePlayers.Contains(PS))
			{
				AlivePlayers.Add(PS);
			}
			if (!PS->IsAlive())
			{
				PS->SetAlive_ServerOnly(true);
			}
		}
		
		if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Controller))
		{
			const FGenericTeamId NewTeamID = GetTeamIdForPlayer(Cast<APlayerController>(Controller));
			TeamAgent->SetGenericTeamId(NewTeamID);
			LOG_NET(POLog, Warning, TEXT("[StageGM] 원활한 이동 플레이어에게 Team ID %d 할당: %s"), NewTeamID.GetId(), *Controller->GetName());
		}
	}
}

UClass* APOStageGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	return PlayerPawnClass
		? *PlayerPawnClass
		: Super::GetDefaultPawnClassForController_Implementation(InController);
}

AActor* APOStageGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (CachedPlayerStarts.Num() == 0)
	{
		CachePlayerStarts();
	}

	TArray<AActor*> Starts;
	Starts.Reserve(CachedPlayerStarts.Num());

	for (const TWeakObjectPtr<AActor>& W : CachedPlayerStarts)
	{
		if (AActor* A = W.Get())
		{
			Starts.Add(A);
		}
	}

	for (int32 i = 0; i < Starts.Num(); ++i)
	{
		const int32 J = FMath::RandRange(i, Starts.Num() - 1);
		Starts.Swap(i, J);
	}

	for (AActor* S : Starts)
	{
		if (!S)
		{
			continue;
		}

		if (UsedPlayerStarts.Contains(S))
		{
			continue;
		}
		UsedPlayerStarts.Add(S);

		if (IsSpawnPointFree(S))
		{
			return S;
		}
		else
		{
			UsedPlayerStarts.Remove(S);
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void APOStageGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

	if (NewPlayer && NewPlayer->GetPawn() && StartSpot)
	{
		UsedPlayerStarts.Add(StartSpot);
	}
}

bool APOStageGameMode::IsSpawnPointFree(AActor* Start) const
{
	if (!Start || !GetWorld())
	{
		return false;
	}

	const FVector Loc = Start->GetActorLocation();
	const float Radius = 120.0f;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SpawnCheck), false);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	const bool bAnyHit = GetWorld()->OverlapAnyTestByObjectType(
		Loc,
		FQuat::Identity,
		ObjParams,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	return !bAnyHit;
}

void APOStageGameMode::CachePlayerStarts()
{
	CachedPlayerStarts.Empty();

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Found);

	for (AActor* Act : Found)
	{
		CachedPlayerStarts.Add(Act);
	}
}

void APOStageGameMode::SpawnAIsOnStage()
{
	if (!HasAuthority() || !AIClass)
	{
		return;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return;
	}

	for (int32 i = 0; i < NumAIsToSpawn; ++i)
	{
		FNavLocation L;
		if (NavSys->GetRandomReachablePointInRadius(FVector::ZeroVector, AIRandomRadius, L))
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			APawn* AIPawn = GetWorld()->SpawnActor<APawn>(AIClass, L.Location, FRotator::ZeroRotator, Params);
			if (AIPawn && !AIPawn->GetController())
			{
				AIPawn->SpawnDefaultController();
			}
		}
	}
}

void APOStageGameMode::NotifyCharacterDied(AController* VictimController, AController* KillerController)
{
	if (!HasAuthority())
	{
		return;
	}

	APOLobbyPlayerState* VictimPS = ToLobbyPS(VictimController);
	APOLobbyPlayerState* KillerPS = ToLobbyPS(KillerController);

	if (VictimPS && KillerPS && KillerPS != VictimPS)
	{
		KillerPS->AddKill_ServerOnly(1);
		LOG_NET(POLog, Log, TEXT("[StageGM] Kill++ : %s"), *KillerPS->GetPlayerName());
	}

	if (!VictimPS)
	{
		return;
	}

	if (VictimPS->IsAlive())
	{
		VictimPS->SetAlive_ServerOnly(false);
		AlivePlayers.Remove(VictimPS);
		LOG_NET(POLog, Log, TEXT("[StageGM] Dead : %s, AliveNow=%d"), *VictimPS->GetPlayerName(), AlivePlayers.Num());
	}

	CompactAlivePlayers();
	TryDecideWinner();
}

void APOStageGameMode::CompactAlivePlayers()
{
	for (auto It = AlivePlayers.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
		}
	}
}

void APOStageGameMode::TryDecideWinner()
{
	if (!bActivatedWithEnoughPlayers)
	{
		return;
	}

	if (const APOStageGameState* GS = GetGameState<APOStageGameState>())
	{
		const EPOStagePhase P = GS->GetPhase();
		if (P != EPOStagePhase::Active && P != EPOStagePhase::RoundEnd)
		{
			return;
		}
	}

	CompactAlivePlayers();

	int32 Count = 0;
	APOLobbyPlayerState* LastAlive = nullptr;

	for (const TWeakObjectPtr<APlayerState>& W : AlivePlayers) // APlayerState로 순회
	{
		if (APlayerState* Base = W.Get())
		{
			if (APOLobbyPlayerState* PS = Cast<APOLobbyPlayerState>(Base))
			{
				if (PS->IsAlive())
				{
					LastAlive = PS;
					++Count;
					if (Count > 1) break;
				}
			}
		}
	}

	if (Count == 1)
	{
		BeginGameEndPhase(LastAlive);
	}
	else if (Count == 0)
	{
		BeginGameEndPhase(nullptr);
	}
}

void APOStageGameMode::NotifySpecialVictory(APlayerState* WinnerPS)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bGameEndStarted)
	{
		LOG_NET(POLog, Log, TEXT("[StageGM] NotifySpecialVictory: Already in GameEnd."));
		return;
	}

	if (WinnerPS && GameState && !GameState->PlayerArray.Contains(WinnerPS))
	{
		LOG_NET(POLog, Warning, TEXT("[StageGM] NotifySpecialVictory: Winner not in PlayerArray. Ignored."));
		return;
	}

	LOG_NET(
		POLog,
		Warning,
		TEXT("[StageGM] SpecialVictory triggered: Winner=%s"),
		WinnerPS ? *WinnerPS->GetPlayerName() : TEXT("None")
	);

	BeginGameEndPhase(WinnerPS);
}

void APOStageGameMode::HandlePhaseChanged(EPOStagePhase NewPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	LOG_NET(
		POLog,
		Warning,
		TEXT("[StageGM] Phase changed -> %s"),
		*StaticEnum<EPOStagePhase>()->GetNameStringByValue(static_cast<int64>(NewPhase))
	);

	if (NewPhase == EPOStagePhase::Active)
	{
		if (APOStageGameState* GS = GetGameState<APOStageGameState>())
		{
			const int32 Alive = CountAliveHumans();
			if (Alive < MinPlayersToStart)
			{
				LOG_NET(POLog, Warning, TEXT("[StageGM] Not enough players (%d/%d). Staying in Prep."), Alive, MinPlayersToStart);
				GS->ServerSetPhase(EPOStagePhase::Prep);
				GS->ServerStartPrepCountdown(PrepSeconds);
				return;
			}

			bActivatedWithEnoughPlayers = true;
			GS->ServerStartStageCountdown(StageSeconds);
		}
	}
}

void APOStageGameMode::HandleStageTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	LOG_NET(POLog, Warning, TEXT("[StageGM] HandleStageTimeExpired: Stage time expired signal received."));
	BeginRoundEndPhase();
}

void APOStageGameMode::ResetAllPlayerStatesForMatchStart()
{
	if (!HasAuthority())
	{
		return;
	}

	if (AGameStateBase* GS = GameState)
	{
		for (APlayerState* PSBase : GS->PlayerArray)
		{
			if (APOLobbyPlayerState* PS = Cast<APOLobbyPlayerState>(PSBase))
			{
				PS->ServerResetForMatchStart();
			}
		}
	}

	if (APOStageGameState* POGS = GetGameState<APOStageGameState>())
	{
		POGS->ServerClearWinner();
	}

	bAIWipedAtRoundEnd = false;
	bGameEndStarted    = false;
}

void APOStageGameMode::BeginRoundEndPhase()
{
	if (!HasAuthority())
	{
		return;
	}

	if (APOStageGameState* GS = GetGameState<APOStageGameState>())
	{
		GS->ServerSetPhase(EPOStagePhase::RoundEnd);
	}

	WipeAllAIsOnStage();
}

void APOStageGameMode::WipeAllAIsOnStage()
{
	if (!HasAuthority())
	{
		return;
	}
	if (bAIWipedAtRoundEnd)
	{
		return;
	}
	bAIWipedAtRoundEnd = true;

	int32 Removed = 0;

	for (TActorIterator<AAIController> It(GetWorld()); It; ++It)
	{
		AAIController* AIC = *It;
		if (!AIC) { continue; }

		if (AIC->IsPlayerController())
		{
			continue;
		}

		APawn* Pawn = AIC->GetPawn();
		if (!Pawn)
		{
			AIC->Destroy();
			continue;
		}

		if (AIClass && !Pawn->IsA(AIClass))
		{
			continue;
		}

		if (AIC->BrainComponent)
		{
			AIC->BrainComponent->StopLogic(TEXT("RoundEnd"));
		}
		AIC->StopMovement();

		AIC->UnPossess();
		Pawn->Destroy();
		AIC->Destroy();

		++Removed;
	}

	LOG_NET(POLog, Warning, TEXT("[StageGM] WipeAllAIsOnStage: Removed AI pawns=%d"), Removed);
}

void APOStageGameMode::BeginGameEndPhase(APlayerState* InWinnerPS)
{
	if (!HasAuthority())
	{
		return;
	}
	if (bGameEndStarted)
	{
		return;
	}
	bGameEndStarted = true;

	if (APOStageGameState* GS = GetGameState<APOStageGameState>())
	{
		GS->ServerSetWinner(InWinnerPS);
		GS->ServerSetPhase(EPOStagePhase::GameEnd);

		LOG_NET(POLog, Warning, TEXT("[StageGM] BeginGameEndPhase: Winner=%s"),
			InWinnerPS ? *InWinnerPS->GetPlayerName() : TEXT("None"));

		StartReturnToLobbyTimer();
	}
}

void APOStageGameMode::StartReturnToLobbyTimer()
{
	if (!HasAuthority())
	{
		return;
	}

	const int32 Delay = FMath::Max(0, GameEndReturnSeconds);
	if (Delay == 0)
	{
		DoReturnToLobby();
		return;
	}

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimer(
			ReturnToLobbyTimerHandle,
			this,
			&APOStageGameMode::DoReturnToLobby,
			Delay,
			false
		);
	}

	LOG_NET(POLog, Warning, TEXT("[StageGM] ReturnToLobby scheduled in %d sec"), Delay);
}

void APOStageGameMode::DoReturnToLobby()
{
	if (!HasAuthority())
	{
		return;
	}

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(ReturnToLobbyTimerHandle);
	}

	FString URL = LobbyMapURL;
	if (URL.IsEmpty())
	{
		URL = TEXT("/Game/All/Game/Levels/L_ServerLobby");
	}

	if (GetNetMode() == NM_ListenServer && !URL.Contains(TEXT("?")))
	{
		URL += TEXT("?listen");
	}

	LOG_NET(
		POLog,
		Warning,
		TEXT("[StageGM] DoReturnToLobby: NOW traveling to lobby -> %s"),
		*URL
	);

	if (UWorld* W = GetWorld())
	{
		W->ServerTravel(URL, false);
	}
}

void APOStageGameMode::EndGameForGimmick(APlayerState* WinnerPS)
{
	if (!HasAuthority())
	{
		return;
	}
	NotifySpecialVictory(WinnerPS);
}

int32 APOStageGameMode::CountAliveHumans() const
{
	int32 Alive = 0;
	for (const TWeakObjectPtr<APlayerState>& W : AlivePlayers)
	{
		if (const APlayerState* Base = W.Get())
		{
			if (const APOLobbyPlayerState* PS = Cast<APOLobbyPlayerState>(Base))
			{
				if (PS->IsAlive())
				{
					++Alive;
				}
			}
		}
	}
	return Alive;
}

int32 APOStageGameMode::CountConnectedHumans() const
{
	int32 Connected = 0;
	if (const AGameStateBase* GS = GameState)
	{
		for (const APlayerState* PSBase : GS->PlayerArray)
		{
			if (PSBase && !PSBase->IsOnlyASpectator())
			{
				++Connected;
			}
		}
	}
	return Connected;
}

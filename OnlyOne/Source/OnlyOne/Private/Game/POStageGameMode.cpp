// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POStageGameMode.h"
#include "game/POStageGameState.h"
#include "game/POLobbyPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "CollisionQueryParams.h"
#include "HAL/CriticalSection.h"
#include "OnlyOne/OnlyOne.h"

APOStageGameMode::APOStageGameMode()
{
	NumAIsToSpawn = 8;
	AIRandomRadius = 2000.0f;
	PrepSeconds = 10;
	StageSeconds = 20;
	bDidSubscribePhase = false;

	GameStateClass = APOStageGameState::StaticClass();
}

void APOStageGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
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

	UsedPlayerStarts.Reset();
	ResetAllPlayerStatesForMatchStart();

	CachePlayerStarts();
	SpawnAIsOnStage();

	if (APOStageGameState* GS = GetGameState<APOStageGameState>())
	{
		if (!bDidSubscribePhase)
		{
			GS->OnPhaseChanged.AddUObject(this, &APOStageGameMode::HandlePhaseChanged);
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

	Super::EndPlay(EndPlayReason);
}

void APOStageGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Seamless 후 늦게 들어온 플레이어
	if (NewPlayer)
	{
		// TODO: 관전 모드 진입 고려
		RestartPlayer(NewPlayer);
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

		{
			FScopeLock Guard(&StartLock);
			if (UsedPlayerStarts.Contains(S))
			{
				continue;
			}
			UsedPlayerStarts.Add(S);
		}

		if (IsSpawnPointFree(S))
		{
			return S;
		}
		else
		{
			FScopeLock Guard(&StartLock);
			UsedPlayerStarts.Remove(S);
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
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

void APOStageGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

	if (NewPlayer && NewPlayer->GetPawn() && StartSpot)
	{
		UsedPlayerStarts.Add(StartSpot);
	}
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
			GS->ServerStartStageCountdown(StageSeconds);
		}
	}
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
}
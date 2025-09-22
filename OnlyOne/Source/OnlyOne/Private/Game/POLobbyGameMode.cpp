// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyGameMode.h"
#include "game/POLobbyGameState.h"
#include "game/POLobbyPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"
#include "OnlyOne/OnlyOne.h"

APOLobbyGameMode::APOLobbyGameMode()
{
	GameStateClass   = APOLobbyGameState::StaticClass();
	PlayerStateClass = APOLobbyPlayerState::StaticClass();
	
	bUseSeamlessTravel = true;

	MaxPlayersInLobby = 8;
	CountdownSecondsDefault = 5;
	bCountdownRunning       = false;
	CountdownRemaining      = 0;
	
}

void APOLobbyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (GameSession)
	{
		GameSession->MaxPlayers = MaxPlayersInLobby;
	}
}

void APOLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (!ErrorMessage.IsEmpty())
	{
		return;
	}
	
	const int32 CurrentPlayers = GetNumPlayers() + NumTravellingPlayers;
	if (CurrentPlayers >= MaxPlayersInLobby)
	{
		ErrorMessage = TEXT("Server is full (max players reached).");
		return;
	}
}

void APOLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	CancelCountdown(TEXT("Player joined"));
	if (AreAllPlayersReady())
	{
		TryStartCountdown();
	}
}

void APOLobbyGameMode::Logout(AController* Exiting)
{
	if (APOLobbyPlayerState* PS = Exiting ? Exiting->GetPlayerState<APOLobbyPlayerState>() : nullptr)
	{
		PS->MulticastPlayerLeftLobby(PS->GetBaseNickname());
	}

	CancelCountdown(TEXT("Player left"));
	
	Super::Logout(Exiting);

	CancelCountdown(TEXT("Ready state changed"));
	if (AreAllPlayersReady())
	{
		TryStartCountdown();
	}
}

bool APOLobbyGameMode::AreAllPlayersReady() const
{
	const AGameStateBase* GS = GameState;
	if (!GS)
	{
		return false;
	}

	int32 TotalPlayers = 0;
	int32 ReadyPlayers = 0;

	for (APlayerState* PS : GS->PlayerArray)
	{
		if (APOLobbyPlayerState* LobbyPS = Cast<APOLobbyPlayerState>(PS))
		{
			++TotalPlayers;
			if (LobbyPS->IsReady())
			{
				++ReadyPlayers;
			}
		}
	}

	if (TotalPlayers < 2)
	{
		return false;
	}

	return (ReadyPlayers == TotalPlayers);
}

void APOLobbyGameMode::TryStartCountdown()
{
	if (bCountdownRunning)
	{
		return;
	}

	bCountdownRunning  = true;
	CountdownRemaining = CountdownSecondsDefault;

	if (APOLobbyGameState* LGS = GetGameState<APOLobbyGameState>())
	{
		LGS->SetAllReady(true);
		LGS->SetCountdownRemaining(CountdownRemaining);
	}

	LOG_NET(POLog, Warning, TEXT("[LobbyGM] All players ready -> start countdown (%d s)"), CountdownRemaining);

	// 1초 간격 틱
	GetWorld()->GetTimerManager().SetTimer(
		StartMatchTimerHandle,
		this,
		&APOLobbyGameMode::TickCountdown,
		1.0f,
		true
	);
}

void APOLobbyGameMode::NotifyReadyStateChanged(APOLobbyPlayerState* /*ChangedPlayer*/, bool /*bNowReady*/)
{
	CancelCountdown(TEXT("Ready state changed"));
	if (AreAllPlayersReady())
	{
		TryStartCountdown();
	}
}


void APOLobbyGameMode::CancelCountdown(const TCHAR* Reason)
{
	if (!bCountdownRunning)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(StartMatchTimerHandle);
	bCountdownRunning  = false;
	CountdownRemaining = 0;

	if (APOLobbyGameState* LGS = GetGameState<APOLobbyGameState>())
	{
		LGS->SetAllReady(false);
		LGS->SetCountdownRemaining(0);
	}

	LOG_NET(POLog, Warning, TEXT("[LobbyGM] Countdown cancelled: %s"), Reason ? Reason : TEXT("Unknown"));
}

void APOLobbyGameMode::TickCountdown()
{
	if (!AreAllPlayersReady())
	{
		CancelCountdown(TEXT("Validation failed during countdown (player left / unready)"));
		return;
	}
	
	--CountdownRemaining;

	if (APOLobbyGameState* LGS = GetGameState<APOLobbyGameState>())
	{
		LGS->SetCountdownRemaining(FMath::Max(CountdownRemaining, 0));
	}

	if (CountdownRemaining > 0)
	{
		LOG_NET(POLog, Warning, TEXT("[LobbyGM] Countdown ticking: %d"), CountdownRemaining);
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(StartMatchTimerHandle);
	bCountdownRunning = false;

	OnCountdownFinished();
}

void APOLobbyGameMode::OnCountdownFinished()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		LOG_NET(POLog, Error, TEXT("[LobbyGM] OnCountdownFinished: World null"));
		return;
	}

	// GameMode는 서버 전용이지만, 혹시 모를 호출 경로 안전 장치
	if (!HasAuthority())
	{
		LOG_NET(POLog, Warning, TEXT("[LobbyGM] OnCountdownFinished called without authority -> abort"));
		return;
	}

	// SeamlessTravel 중복 방지
	if (World->IsInSeamlessTravel())
	{
		LOG_NET(POLog, Warning, TEXT("[LobbyGM] SeamlessTravel in progress -> delay retry"));
		World->GetTimerManager().SetTimer(DelayedTravelHandle, this, &APOLobbyGameMode::OnCountdownFinished, 0.25f, false);
		return;
	}

	// 아직 로딩 중(이전 travel) 플레이어 존재 시 재시도
	if (NumTravellingPlayers > 0)
	{
		LOG_NET(POLog, Warning, TEXT("[LobbyGM] NumTravellingPlayers=%d -> delay retry"), NumTravellingPlayers);
		World->GetTimerManager().SetTimer(DelayedTravelHandle, this, &APOLobbyGameMode::OnCountdownFinished, 0.25f, false);
		return;
	}

	static const FString TargetMapPath = TEXT("/Game/Levels/L_TestMainLevel");
	// ListenServer 최초 open 시 이미 ?listen 사용했으므로 재여행에서는 불필요
	const FString TravelURL = TargetMapPath; 

	LOG_NET(POLog, Warning, TEXT("[LobbyGM] Travelling to %s (Players=%d)"), *TravelURL, GetNumPlayers());

	World->ServerTravel(TravelURL, /*bAbsolute*/ false);
}
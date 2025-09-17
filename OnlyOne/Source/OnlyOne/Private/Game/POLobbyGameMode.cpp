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
	
	bUseSeamlessTravel = false;

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
	// 실제 레벨 이동 없이, 구조만 동일하게 LOG로 검증
	// 예시: LOG_NET(POLog, Warning, TEXT("Player %d nickname initialized from GI: %s"), GetPlayerId(), *BaseNickname);
	// → 형식을 맞춰 경고 로그로 출력(내용만 다름)
	LOG_NET(POLog, Warning, TEXT("Lobby countdown finished -> (Simulated) Start Game Level Travel"));
}
// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyGameMode.h"
#include "game/POLobbyGameState.h"
#include "game/POLobbyPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameSession.h"

APOLobbyGameMode::APOLobbyGameMode()
{
	GameStateClass   = APOLobbyGameState::StaticClass();
	PlayerStateClass = APOLobbyPlayerState::StaticClass();
	
	bUseSeamlessTravel = false;

	MaxPlayersInLobby = 8;
	
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

	if (APOLobbyPlayerState* PS = NewPlayer ? NewPlayer->GetPlayerState<APOLobbyPlayerState>() : nullptr)
	{
		// NOTE: 플레이어가 레디를 눌러야 준비 완료가 되야 합니다.
		//PS->ServerSetReady();
	}
}

void APOLobbyGameMode::Logout(AController* Exiting)
{
	if (APOLobbyPlayerState* PS = Exiting ? Exiting->GetPlayerState<APOLobbyPlayerState>() : nullptr)
	{
		PS->MulticastPlayerLeftLobby(PS->GetBaseNickname());
	}
	
	Super::Logout(Exiting);
}
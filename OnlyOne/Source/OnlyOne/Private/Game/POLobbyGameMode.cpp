// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyGameMode.h"
#include "game/POLobbyGameState.h"
#include "game/POLobbyPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

APOLobbyGameMode::APOLobbyGameMode()
{
	GameStateClass   = APOLobbyGameState::StaticClass();
	PlayerStateClass = APOLobbyPlayerState::StaticClass();
	
	bUseSeamlessTravel = false;
}

void APOLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (APOLobbyPlayerState* PS = NewPlayer ? NewPlayer->GetPlayerState<APOLobbyPlayerState>() : nullptr)
	{
		PS->ServerSetReady(false);
	}
}

void APOLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyGameState.h"
#include "Net/UnrealNetwork.h"

APOLobbyGameState::APOLobbyGameState()
{
	CountdownRemaining = 0;
	bAllReady = false;
}

void APOLobbyGameState::SetCountdownRemaining(int32 InRemaining)
{
	if (HasAuthority())
	{
		CountdownRemaining = InRemaining;
	}
}

void APOLobbyGameState::SetAllReady(bool bInAllReady)
{
	if (HasAuthority())
	{
		bAllReady = bInAllReady;
	}
}

void APOLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APOLobbyGameState, CountdownRemaining);
	DOREPLIFETIME(APOLobbyGameState, bAllReady);
}
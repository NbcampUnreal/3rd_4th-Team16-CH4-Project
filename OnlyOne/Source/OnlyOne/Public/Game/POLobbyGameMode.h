// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "POLobbyGameMode.generated.h"

class APOLobbyPlayerState;
class APOLobbyGameState;

/**
 * 
 */
UCLASS()
class ONLYONE_API APOLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APOLobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
};

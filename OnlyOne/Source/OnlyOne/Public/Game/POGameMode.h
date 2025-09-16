// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameMode.h"
#include "POGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APOGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

private:
	FGenericTeamId GetTeamIdForPlayer(APlayerController* PlayerController);

	int32 PlayerCount = 0;
};

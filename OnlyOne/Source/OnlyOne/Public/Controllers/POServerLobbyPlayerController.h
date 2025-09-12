// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "POServerLobbyPlayerController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerReadyStateChanged);

UCLASS()
class ONLYONE_API APOServerLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnPlayerReadyStateChanged OnReadyStateChanged;
	
};

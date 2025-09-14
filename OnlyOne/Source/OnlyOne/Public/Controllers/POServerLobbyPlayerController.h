// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "POServerLobbyPlayerController.generated.h"

/**
 * 
 */

class UPOServerLobbyWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReadyStateChanged, const FJoinServerData&, PlayerData, bool, bIsReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinLobby, const FJoinServerData&, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeaveLobby, const FJoinServerData&, LeftPlayer);


UCLASS()
class ONLYONE_API APOServerLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnPlayerReady OnPlayerReady;
	
	UPROPERTY()
	FOnPlayerReadyStateChanged OnReadyStateChanged;

	UPROPERTY()
	FOnPlayerJoinLobby OnPlayerJoinLobby;

	UPROPERTY()
	FOnPlayerLeaveLobby OnPlayerLeaveLobby;

protected:
	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPOServerLobbyWidget> ServerLobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UPOServerLobbyWidget> ServerLobbyWidget;

	UFUNCTION()
	void ShowLobbyWidget();
};

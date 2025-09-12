// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POServerLobbyPlayerElementWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOServerLobbyPlayerElementWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerReadyStateTextBlock;

	void SetPlayerName(const FString& PlayerName);
	void SetPlayerReadyState(bool bIsReady);
};

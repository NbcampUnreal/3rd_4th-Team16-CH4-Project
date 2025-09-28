// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

class UPOKillFeedWidget;
class UPOPlayerStateListWidget;

#include "POUITestPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APOUITestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI|KillFeed")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidgetInstance;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "POMainMenuPlayerController.generated.h"

class UPOMainMenuWidget;

/**
 * 
 */
UCLASS()
class ONLYONE_API APOMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APOMainMenuPlayerController();

protected:
	virtual void BeginPlay() override;

	// MainMenu Widget Blueprint Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOMainMenuWidget> MainMenuWidgetClass;

	// MainMenu Widget Instance
	UPROPERTY()
	TObjectPtr<UPOMainMenuWidget> MainMenuWidget;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();
};

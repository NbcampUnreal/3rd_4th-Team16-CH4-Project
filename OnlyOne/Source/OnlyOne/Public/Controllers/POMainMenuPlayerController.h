// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "POMainMenuPlayerController.generated.h"

class UPOJoinServerWidget;
class UPOMainMenuWidget;

/**
 * 
 */

USTRUCT()
struct FJoinServerData
{
	GENERATED_BODY()

	FString Name = TEXT("Player");
	FString DisplayNickname = TEXT("");
	FString IPAddress = TEXT("127.0.0.1");
};

UCLASS()
class ONLYONE_API APOMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APOMainMenuPlayerController();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowJoinServer();

	UFUNCTION()
	void OnJoinServer(const FJoinServerData& JoinServerData);
	
protected:
	virtual void BeginPlay() override;

	// MainMenu Widget 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOMainMenuWidget> MainMenuWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UPOMainMenuWidget> MainMenuWidget;

	// Join Server Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOJoinServerWidget> JoinServerWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UPOJoinServerWidget> JoinServerWidget;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();
};

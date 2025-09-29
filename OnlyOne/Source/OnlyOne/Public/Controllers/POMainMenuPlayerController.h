// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/POUIStackingInterface.h"
#include "POMainMenuPlayerController.generated.h"

class UPOUIStackingComponent;
class UPOHostServerWidget;
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
class ONLYONE_API APOMainMenuPlayerController : public APlayerController, public IPOUIStackingInterface
{
	GENERATED_BODY()

public:
	APOMainMenuPlayerController();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowJoinServer();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowHostServer();

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool HideLastUI();
	
	UFUNCTION()
	void OnJoinServer(FJoinServerData& JoinServerData);

	UFUNCTION()
	void OnHostServer(FJoinServerData& HostServerData);

	FORCEINLINE virtual UPOUIStackingComponent* GetUIStackingComponent() const override { return UIStackingComponent; }
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UPOUIStackingComponent> UIStackingComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOMainMenuWidget> MainMenuWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UPOMainMenuWidget> MainMenuWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOJoinServerWidget> JoinServerWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UPOJoinServerWidget> JoinServerWidget;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOHostServerWidget> HostServerWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UPOHostServerWidget> HostServerWidget;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();
};

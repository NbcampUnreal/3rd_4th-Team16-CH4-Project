// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POServerLobbyWidget.generated.h"

class UScrollBox;
struct FJoinServerData;
class UButton;
class UPOServerLobbyPlayerElementWidget;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOServerLobbyWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPOServerLobbyPlayerElementWidget> PlayerSlotClass;

	UPROPERTY(EditAnywhere)
	TMap<FString, TObjectPtr<UPOServerLobbyPlayerElementWidget>> PlayerSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> PlayerListScrollBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

private:
	UFUNCTION()
	void OnReadyButtonClicked();

	UFUNCTION()
	void OnJoinPlayer(const FJoinServerData& InNewPlayer);

	UFUNCTION()
	void OnExitPlayer(const FJoinServerData& InExitPlayer);

	UFUNCTION()
	void OnReadyPlayer(const FJoinServerData& InReadyPlayer, bool bIsReady);
};

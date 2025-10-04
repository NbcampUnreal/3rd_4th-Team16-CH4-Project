// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"

class UPOCustomButton;

#include "POInGameMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOInGameMenuWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// ESC 등 키 입력을 처리하기 위해 오버라이드
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOCustomButton> BackToGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOCustomButton> SettingsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOCustomButton> BackToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOCustomButton> QuitButton;

	UFUNCTION()
	void OnBackToGameClicked(UPOCustomButton* ClickedButton);

	UFUNCTION()
	void OnSettingsClicked(UPOCustomButton* ClickedButton);

	UFUNCTION()
	void OnBackToMainMenuClicked(UPOCustomButton* ClickedButton);

	UFUNCTION()
	void OnQuitClicked(UPOCustomButton* ClickedButton);
};

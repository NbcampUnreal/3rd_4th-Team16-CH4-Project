// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/POInGameMenuWidget.h"
#include "UI/Common/POCustomButton.h"
#include "Controllers/POPlayerController.h"
#include "Controllers/Components/POUIStackingComponent.h"

void UPOInGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 포커스 가능 설정은 유지
	SetIsFocusable(true);

	if (BackToGameButton && !BackToGameButton->OnCustomButtonClicked.IsAlreadyBound(this, &UPOInGameMenuWidget::OnBackToGameClicked))
	{
		BackToGameButton->SetButtonText(FText::FromString(TEXT("Back to Game")));
		BackToGameButton->OnCustomButtonClicked.AddDynamic(this, &UPOInGameMenuWidget::OnBackToGameClicked);
	}

	if (SettingsButton && !SettingsButton->OnCustomButtonClicked.IsAlreadyBound(this, &UPOInGameMenuWidget::OnSettingsClicked))
	{
		SettingsButton->SetButtonText(FText::FromString(TEXT("Settings")));
		SettingsButton->OnCustomButtonClicked.AddDynamic(this, &UPOInGameMenuWidget::OnSettingsClicked);
	}

	if (BackToMainMenuButton && !BackToMainMenuButton->OnCustomButtonClicked.IsAlreadyBound(this, &UPOInGameMenuWidget::OnBackToMainMenuClicked))
	{
		BackToMainMenuButton->SetButtonText(FText::FromString(TEXT("Back to Main Menu")));
		BackToMainMenuButton->OnCustomButtonClicked.AddDynamic(this, &UPOInGameMenuWidget::OnBackToMainMenuClicked);
	}

	if (QuitButton && !QuitButton->OnCustomButtonClicked.IsAlreadyBound(this, &UPOInGameMenuWidget::OnQuitClicked))
	{
		QuitButton->SetButtonText(FText::FromString(TEXT("Quit")));
		QuitButton->OnCustomButtonClicked.AddDynamic(this, &UPOInGameMenuWidget::OnQuitClicked);
	}
}

void UPOInGameMenuWidget::NativeDestruct()
{
	if (BackToGameButton)
	{
		BackToGameButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOInGameMenuWidget::OnBackToGameClicked);
	}
	if (SettingsButton)
	{
		SettingsButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOInGameMenuWidget::OnSettingsClicked);
	}
	if (BackToMainMenuButton)
	{
		BackToMainMenuButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOInGameMenuWidget::OnBackToMainMenuClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOInGameMenuWidget::OnQuitClicked);
	}

	Super::NativeDestruct();
}

void UPOInGameMenuWidget::OnBackToGameClicked(UPOCustomButton* ClickedButton)
{
	UE_LOG(LogTemp, Log, TEXT("InGameMenu: Back to Game clicked"));
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		if (UPOUIStackingComponent* Stacking = PC->GetUIStackingComponent())
		{
			Stacking->PopWidget();
		}
	}
}

void UPOInGameMenuWidget::OnSettingsClicked(UPOCustomButton* ClickedButton)
{
	UE_LOG(LogTemp, Log, TEXT("InGameMenu: Settings clicked"));
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		PC->ShowSettingWidget();
	}
}

void UPOInGameMenuWidget::OnBackToMainMenuClicked(UPOCustomButton* ClickedButton)
{
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		PC->ShowRetunToLobbyWidget();
	}
}

void UPOInGameMenuWidget::OnQuitClicked(UPOCustomButton* ClickedButton)
{
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		PC->ShowExitGameWidget();
	}
}

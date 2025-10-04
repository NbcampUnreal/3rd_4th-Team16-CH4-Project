// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/POInGameMenuWidget.h"
#include "UI/Common/POCustomButton.h"
#include "Controllers/POPlayerController.h"
#include "Controllers/Components/POUIStackingComponent.h"

void UPOInGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ESC 키 입력을 받을 수 있도록 포커스 가능하게 설정
	SetIsFocusable(true);

	if (BackToGameButton)
	{
		BackToGameButton->SetButtonText(FText::FromString(TEXT("Back to Game")));
		BackToGameButton->OnCustomButtonClicked.AddDynamic(this, &UPOInGameMenuWidget::OnBackToGameClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->SetButtonText(FText::FromString(TEXT("Settings")));
		SettingsButton->OnCustomButtonClicked.AddDynamic(this, &UPOInGameMenuWidget::OnSettingsClicked);
	}

	if (BackToMainMenuButton)
	{
		BackToMainMenuButton->SetButtonText(FText::FromString(TEXT("Back to Main Menu")));
		BackToMainMenuButton->OnCustomButtonClicked.AddDynamic(this, &UPOInGameMenuWidget::OnBackToMainMenuClicked);
	}

	if (QuitButton)
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

FReply UPOInGameMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Escape)
	{
		if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
		{
			if (UPOUIStackingComponent* Stacking = PC->GetUIStackingComponent())
			{
				Stacking->PopWidget();
				return FReply::Handled();
			}
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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
}

void UPOInGameMenuWidget::OnBackToMainMenuClicked(UPOCustomButton* ClickedButton)
{
	UE_LOG(LogTemp, Log, TEXT("InGameMenu: Back to Main Menu clicked"));
}

void UPOInGameMenuWidget::OnQuitClicked(UPOCustomButton* ClickedButton)
{
	UE_LOG(LogTemp, Log, TEXT("InGameMenu: Quit clicked"));
}

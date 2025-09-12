// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ServerLobbyMenu/POServerLobbyPlayerElementWidget.h"

#include "Components/TextBlock.h"

void UPOServerLobbyPlayerElementWidget::SetPlayerName(const FString& PlayerName)
{
	PlayerNameTextBlock->SetText(FText::FromString(PlayerName));
}

void UPOServerLobbyPlayerElementWidget::SetPlayerReadyState(bool bIsReady)
{
	if (bIsReady)
	{
		PlayerReadyStateTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PlayerReadyStateTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}
}

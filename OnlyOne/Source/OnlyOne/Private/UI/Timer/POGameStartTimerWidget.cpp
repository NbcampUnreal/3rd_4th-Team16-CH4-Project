// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timer/POGameStartTimerWidget.h"

#include "Controllers/POServerLobbyPlayerController.h"

void UPOGameStartTimerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		if (!PC->OnGameStartTimerChanged.IsAlreadyBound(this, &ThisClass::SetTimerText))
		{
			PC->OnGameStartTimerChanged.AddDynamic(this, &ThisClass::SetTimerText);
		}
	}
}

void UPOGameStartTimerWidget::NativeDestruct()
{
	if ( APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		if (PC->OnGameStartTimerChanged.IsAlreadyBound(this, &ThisClass::SetTimerText))
		{
			PC->OnGameStartTimerChanged.RemoveDynamic(this, &ThisClass::SetTimerText);
		}
	}
	
	Super::NativeDestruct();
}

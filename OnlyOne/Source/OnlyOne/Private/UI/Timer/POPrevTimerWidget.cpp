// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timer/POPrevTimerWidget.h"

#include "Game/POStageGameState.h"

void UPOPrevTimerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (APOStageGameState* GameState = GetWorld()->GetGameState<APOStageGameState>())
	{
		GameState->OnPrepTimeUpdated.AddUObject(this, &UPOPrevTimerWidget::SetTimerText);
	}
}

void UPOPrevTimerWidget::NativeDestruct()
{
	if (APOStageGameState* GameState = GetWorld()->GetGameState<APOStageGameState>())
	{
		GameState->OnPrepTimeUpdated.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

#include "UI/PlayerCounter/POPlayerCounterWidget.h"
#include "Controllers/POPlayerController.h"
#include "Game/POStageGameState.h"
#include "TimerManager.h"

void UPOPlayerCounterWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		PC->OnChangedAlivePlayer.AddUniqueDynamic(this, &ThisClass::SetTextBox);
	}
	
	if (const UWorld* W = GetWorld())
	{
		if (const APOStageGameState* GS = W->GetGameState<APOStageGameState>())
		{
			LastShownAlive = GS->AlivePlayerCount;
			SetTextBox(LastShownAlive);
		}
	}
	
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimer(
			AliveUiTickHandle,
			this,
			&ThisClass::TickAliveUi,
			0.25f,
			true,
			0.25f
		);
	}
}

void UPOPlayerCounterWidget::NativeDestruct()
{
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(AliveUiTickHandle);
	}


	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		PC->OnChangedAlivePlayer.RemoveDynamic(this, &ThisClass::SetTextBox);
	}

	Super::NativeDestruct();
}

void UPOPlayerCounterWidget::TickAliveUi()
{
	const UWorld* W = GetWorld();
	if (!W) return;

	const APOStageGameState* GS = W->GetGameState<APOStageGameState>();
	if (!GS) return;

	const int32 Current = GS->AlivePlayerCount;
	if (LastShownAlive != Current)
	{
		LastShownAlive = Current;
		SetTextBox(Current);
	}
}

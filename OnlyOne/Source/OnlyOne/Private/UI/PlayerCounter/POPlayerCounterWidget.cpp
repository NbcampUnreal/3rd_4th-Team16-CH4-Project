// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerCounter/POPlayerCounterWidget.h"

#include "Controllers/POPlayerController.h"

void UPOPlayerCounterWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		if (!PC->OnChangedAlivePlayer.IsAlreadyBound(this, &ThisClass::SetTextBox))
		{
			PC->OnChangedAlivePlayer.AddDynamic(this, &ThisClass::SetTextBox);
		}
	}
}

void UPOPlayerCounterWidget::NativeDestruct()
{
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		if (PC->OnChangedAlivePlayer.IsAlreadyBound(this, &ThisClass::SetTextBox))
		{
			PC->OnChangedAlivePlayer.RemoveDynamic(this, &ThisClass::SetTextBox);
		}
	}
	
	Super::NativeDestruct();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SmokeCounter/POSmokeCounterWidget.h"
#include "Components/TextBlock.h"
#include "Controllers/POPlayerController.h"
#include "OnlyOne/OnlyOne.h"

void UPOSmokeCounterWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		if (!PC->OnSmokeCountChanged.IsAlreadyBound(this, &ThisClass::SetSmokeCount))
		{
			PC->OnSmokeCountChanged.AddDynamic(this, &ThisClass::SetSmokeCount);
		}
	}
	UE_LOG(POLog, Log, TEXT("[SmokeCounterWidget] Initialized"));
}

void UPOSmokeCounterWidget::NativeDestruct()
{
	if (APOPlayerController* PC = Cast<APOPlayerController>(GetOwningPlayer()))
	{
		if (PC->OnSmokeCountChanged.IsAlreadyBound(this, &ThisClass::SetSmokeCount))
		{
			PC->OnSmokeCountChanged.RemoveDynamic(this, &ThisClass::SetSmokeCount);
		}
	}
	Super::NativeDestruct();
}

void UPOSmokeCounterWidget::SetSmokeCount(int32 NewCount)
{
	if (SmokeText)
	{
		SmokeText->SetText(FText::AsNumber(NewCount));
	}
}

void UPOSmokeCounterWidget::SetSmokeText(const FText& InText)
{
	if (SmokeText)
	{
		SmokeText->SetText(InText);
	}
}

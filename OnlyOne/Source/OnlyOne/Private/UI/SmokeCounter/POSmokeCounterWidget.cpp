// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SmokeCounter/POSmokeCounterWidget.h"
#include "Components/TextBlock.h"

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

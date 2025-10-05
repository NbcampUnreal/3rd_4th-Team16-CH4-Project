// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timer/POTimerWidget.h"
#include "Components/TextBlock.h"

void UPOTimerWidget::SetTimerText(const FText& InText)
{
	if (TimerText)
	{
		if (TimerText)
		TimerText->SetText(InText);
	}
}

void UPOTimerWidget::SetTimerText(int32 NewTimeSeconds)
{
	if (NewTimeSeconds == -1)
	{
		SetTimerText(FText::FromString(TEXT("")));
	}
	SetTimerText(FText::AsNumber(NewTimeSeconds));
}

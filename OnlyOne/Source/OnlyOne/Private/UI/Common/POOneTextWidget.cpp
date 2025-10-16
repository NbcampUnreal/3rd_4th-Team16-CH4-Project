// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POOneTextWidget.h"

#include "Components/TextBlock.h"

#define ENSURE_VALID_WIDGET(ptr) \
if (!(ptr) || !(ptr)->IsValidLowLevelFast() || !IsValid(ptr)) { \
UE_LOG(LogTemp, Verbose, TEXT("POOneTextWidget: target invalid, skip")); \
return; \
}

void UPOOneTextWidget::SetTextBox(int32 NewCount)
{
	if (!IsValid(this))
	{
		return;
	}

	ENSURE_VALID_WIDGET(MainText);
	MainText->SetText(FText::AsNumber(NewCount));
}

void UPOOneTextWidget::SetTextBox(const FText& InText)
{
	if (!IsValid(this))
	{
		return;
	}

	ENSURE_VALID_WIDGET(MainText);
	MainText->SetText(InText);
}

#undef ENSURE_VALID_WIDGET
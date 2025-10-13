// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POOneTextWidget.h"

#include "Components/TextBlock.h"

void UPOOneTextWidget::SetTextBox(int32 NewCount)
{
	if (MainText)
	{
		MainText->SetText(FText::AsNumber(NewCount));
	}
}

void UPOOneTextWidget::SetTextBox(const FText& InText)
{
	if (MainText)
	{
		MainText->SetText(InText);
	}
}
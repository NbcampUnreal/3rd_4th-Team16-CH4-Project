// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KillFeed/POKillFeedElementWidget.h"

void UPOKillFeedElementWidget::SetKillFeedTexts(const FText& InKillerText, const FText& InVictimText)
{
	if (KillerText)
	{
		KillerText->SetText(InKillerText);
	}

	if (VictimText)
	{
		VictimText->SetText(InVictimText);
	}

	AddToViewport();
}

void UPOKillFeedElementWidget::RemoveKillFeedEntry()
{
	RemoveFromParent();	
}

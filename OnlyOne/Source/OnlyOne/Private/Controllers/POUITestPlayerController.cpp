// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/POUITestPlayerController.h"
#include "UI/KillFeed/POKillFeedWidget.h"
#include "Blueprint/UserWidget.h"
#include "InputCoreTypes.h"

void APOUITestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (KillFeedWidgetClass && !KillFeedWidget)
	{
		KillFeedWidget = CreateWidget<UPOKillFeedWidget>(this, KillFeedWidgetClass);
		if (KillFeedWidget)
		{
			KillFeedWidget->AddToViewport(/*ZOrder=*/50);
		}
	}
}

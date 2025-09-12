// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/POMainMenuPlayerController.h"
#include "UI/MainMenu/POMainMenuWidget.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/MainMenu/POJoinServerWidget.h"
#include "Game/POGameInstance.h"

APOMainMenuPlayerController::APOMainMenuPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void APOMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		ShowMainMenu();
	}
}

void APOMainMenuPlayerController::ShowMainMenu()
{
	if (MainMenuWidgetClass)
	{
		if (!MainMenuWidget)
		{
			MainMenuWidget = CreateWidget<UPOMainMenuWidget>(this, MainMenuWidgetClass);
		}

		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void APOMainMenuPlayerController::HideMainMenu()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
		SetInputMode(FInputModeGameOnly());
	}
}

void APOMainMenuPlayerController::ShowJoinServer()
{
	if (JoinServerWidgetClass)
	{
		if (!JoinServerWidget)
		{
			JoinServerWidget = CreateWidget<UPOJoinServerWidget>(this, JoinServerWidgetClass);
			JoinServerWidget->AddToViewport();
			SetInputMode(FInputModeUIOnly());
		}

		if (JoinServerWidget)
		{
			JoinServerWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void APOMainMenuPlayerController::OnJoinServer(const FJoinServerData& JoinServerData)
{
	UE_LOG(POLog, Log, TEXT("OnJoinServer : Name=%s, IPAddress=%s"), *JoinServerData.Name, *JoinServerData.IPAddress);

	if (UPOGameInstance* GI = GetGameInstance<UPOGameInstance>())
	{
		GI->SetPendingProfile(JoinServerData.Name, JoinServerData.IPAddress);
	}
}

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
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenu/POHostServerWidget.h"

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
			SetShowMouseCursor(true);
		}
	}
}

void APOMainMenuPlayerController::HideMainMenu()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
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

void APOMainMenuPlayerController::ShowHostServer()
{
	if (HostServerWidgetClass)
	{
		if (!HostServerWidget)
		{
			HostServerWidget = CreateWidget<UPOHostServerWidget>(this, HostServerWidgetClass);
			HostServerWidget->AddToViewport();
			SetInputMode(FInputModeUIOnly());
		}

		if (HostServerWidget)
		{
			HostServerWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void APOMainMenuPlayerController::OnJoinServer(FJoinServerData& JoinServerData)
{
	UE_LOG(POLog, Log, TEXT("OnJoinServer : Name=%s, IPAddress=%s"), *JoinServerData.Name, *JoinServerData.IPAddress);

	if (JoinServerData.IPAddress.IsEmpty())
	{
		UE_LOG(POLog, Warning, TEXT("IP Address cannot be empty"));
		return;
	}
	
	if (UPOGameInstance* GI = GetGameInstance<UPOGameInstance>())
	{
		GI->SetPendingProfile(JoinServerData.Name, JoinServerData.IPAddress);
	}
	
	FString TravelURL = JoinServerData.IPAddress;
	if (!TravelURL.Contains(":"))
	{
		TravelURL += ":7777";
	}

	// 플레이어 이름을 URL 옵션으로 추가
	TravelURL += FString::Printf(TEXT("?Name=%s"), *JoinServerData.Name);

	// UI 정리
	HideMainMenu();
	if (JoinServerWidget)
	{
		JoinServerWidget->RemoveFromParent();
	}

	// 서버 접속
	ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
}

void APOMainMenuPlayerController::OnHostServer(FJoinServerData& HostServerData)
{
	if (UPOGameInstance* GI = GetGameInstance<UPOGameInstance>())
	{
		GI->SetPendingProfile(HostServerData.Name, HostServerData.IPAddress);
	}
	
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, TEXT("L_ServerLobby"), true, TEXT("listen"));
	}
}

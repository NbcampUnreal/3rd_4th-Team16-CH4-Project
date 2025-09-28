// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/POMainMenuPlayerController.h"
#include "UI/MainMenu/POMainMenuWidget.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "Controllers/Components/POUIStackingComponent.h"
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

	UIStackingComponent = CreateDefaultSubobject<UPOUIStackingComponent>(TEXT("UI Stacking Component"));
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
			UIStackingComponent->SetDefaultWidget(MainMenuWidget);
		}
	}
}

void APOMainMenuPlayerController::HideMainMenu()
{
	if (IsValid(MainMenuWidget) && IsValid(UIStackingComponent))
	{
		UIStackingComponent->PopWidget();
	}
}

void APOMainMenuPlayerController::ShowJoinServer()
{
	if (JoinServerWidgetClass)
	{
		if (!JoinServerWidget)
		{
			JoinServerWidget = CreateWidget<UPOJoinServerWidget>(this, JoinServerWidgetClass);
		}

		if (JoinServerWidget)
		{
			UIStackingComponent->PushWidget(JoinServerWidget);
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
		}

		if (HostServerWidget)
		{
			UIStackingComponent->PushWidget(HostServerWidget);
		}
	}
}

bool APOMainMenuPlayerController::HideLastUI()
{
	if (IsValid(UIStackingComponent))
	{
		UIStackingComponent->PopWidget();
		return true;
	}
	return false;
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
		GI->SetPendingProfile(JoinServerData);
	}
	
	FString TravelURL = JoinServerData.IPAddress;
	if (!TravelURL.Contains(":"))
	{
		TravelURL += ":7777";
	}

	// 플레이어 이름을 URL 옵션으로 추가
	TravelURL += FString::Printf(TEXT("?Name=%s"), *JoinServerData.Name);

	// UI 스택 전체 정리 (레벨 이동 전 모든 UI 제거)
	if (UIStackingComponent)
	{
		UIStackingComponent->ClearStack();
	}

	// 서버 접속
	ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
}

void APOMainMenuPlayerController::OnHostServer(FJoinServerData& HostServerData)
{
	if (UPOGameInstance* GI = GetGameInstance<UPOGameInstance>())
	{
		GI->SetPendingProfile(HostServerData);
	}
	
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, TEXT("L_ServerLobby"), true, TEXT("listen"));
	}
}

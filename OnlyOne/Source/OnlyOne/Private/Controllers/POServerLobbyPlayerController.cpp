// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/POServerLobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "UI/ServerLobbyMenu/POServerLobbyWidget.h"
#include "game/POLobbyPlayerState.h"

void APOServerLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		ShowLobbyWidget();
	}
}

void APOServerLobbyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 해당 코드는 어떤 시점에서 실행되는지 명확하지 않아 주석 처리함.
	/*if (IsLocalPlayerController())
	{
		if (APOLobbyPlayerState* PS = GetPlayerState<APOLobbyPlayerState>())
		{
			PS->InitNicknameFromGameInstanceOnce();
		}
	}*/
}

void APOServerLobbyPlayerController::ShowLobbyWidget()
{
	if (ServerLobbyWidgetClass)
	{
		if (!ServerLobbyWidget)
		{
			ServerLobbyWidget = CreateWidget<UPOServerLobbyWidget>(this, ServerLobbyWidgetClass);
		}

		if (ServerLobbyWidget)
		{
			ServerLobbyWidget->AddToViewport();
			SetInputMode(FInputModeUIOnly());
		}
	}
}

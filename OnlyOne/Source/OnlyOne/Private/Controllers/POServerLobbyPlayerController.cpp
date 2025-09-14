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

	OnPlayerReady.AddDynamic(this, &APOServerLobbyPlayerController::OnClickedReadyButton);
}

void APOServerLobbyPlayerController::BeginDestroy()
{
	// OnPlayerReady 델리게이트에서 바인딩된 모든 함수를 제거하여 잠재적인 메모리 누수를 방지합니다.
	OnPlayerReady.Clear();
	OnPlayerJoinLobby.Clear();
	OnPlayerLeaveLobby.Clear();
	OnReadyStateChanged.Clear();
	
	Super::BeginDestroy();
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

void APOServerLobbyPlayerController::OnClickedReadyButton()
{
	if (APOLobbyPlayerState* PS = GetPlayerState<APOLobbyPlayerState>())
	{
		PS->ToggleReady();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ServerLobbyMenu/POServerLobbyWidget.h"

#include "Controllers/POServerLobbyPlayerController.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/ServerLobbyMenu/POServerLobbyPlayerElementWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"

void UPOServerLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TestJoinButton)
	{
		TestJoinButton->OnClicked.AddDynamic(this, &UPOServerLobbyWidget::TestJoinButtonClicked);
	}

	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnReadyStateChanged.AddDynamic(this, &UPOServerLobbyWidget::OnReadyPlayer);
	}
}

void UPOServerLobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPOServerLobbyWidget::OnReadyButtonClicked()
{
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerReady.Broadcast();
		UE_LOG(POLog, Log, TEXT("Ready Button Clicked"));
	}
}

void UPOServerLobbyWidget::OnJoinPlayer(FJoinServerData& InNewPlayer)
{
	if (!PlayerSlotClass)
	{
		UE_LOG(POLog, Warning, TEXT("PlayerSlotClass is not set!"));
		return;
	}

	FString PlayerKey = InNewPlayer.Name;

	if (PlayerSlots.Contains(PlayerKey))
	{
		UE_LOG(POLog, Warning, TEXT("Player slot already exists for key: %s"), *PlayerKey);
		return;
	}

	if (UPOServerLobbyPlayerElementWidget* NewPlayerSlot = CreateWidget<UPOServerLobbyPlayerElementWidget>(this, PlayerSlotClass))
	{
		PlayerSlots.Add(PlayerKey, NewPlayerSlot);
		
		NewPlayerSlot->AddToViewport();
		NewPlayerSlot->SetPlayerName(InNewPlayer.Name);
		PlayerListScrollBox->AddChild(NewPlayerSlot);
		
		UE_LOG(POLog, Log, TEXT("Created and stored player slot for player: %s"), *PlayerKey);
	}
	else
	{
		UE_LOG(POLog, Error, TEXT("Failed to create player slot widget"));
	}
}

void UPOServerLobbyWidget::OnExitPlayer(FJoinServerData& InExitPlayer)
{
	if (PlayerSlots.Contains(InExitPlayer.Name))
	{
		if (UPOServerLobbyPlayerElementWidget* PlayerSlot = PlayerSlots[InExitPlayer.Name])
		{
			PlayerSlot->RemoveFromParent();
			PlayerSlots.Remove(InExitPlayer.Name);
			UE_LOG(POLog, Log, TEXT("Removed player slot for player: %s"), *InExitPlayer.Name);
		}
	}
	else
	{
		UE_LOG(POLog, Warning, TEXT("No player slot found for player: %s"), *InExitPlayer.Name);
	}
}

void UPOServerLobbyWidget::OnReadyPlayer(const FJoinServerData& InReadyPlayer, bool bIsReady)
{
	if (PlayerSlots.Contains(InReadyPlayer.Name))
	{
		if (UPOServerLobbyPlayerElementWidget* PlayerSlot = PlayerSlots[InReadyPlayer.Name])
		{
			PlayerSlot->SetPlayerReadyState(bIsReady);
			UE_LOG(POLog, Log, TEXT("Set ready state for player: %s to %s"), *InReadyPlayer.Name, bIsReady ? TEXT("Ready") : TEXT("Not Ready"));
		}
	}
	else
	{
		UE_LOG(POLog, Warning, TEXT("No player slot found for player: %s"), *InReadyPlayer.Name);
	}
}

void UPOServerLobbyWidget::TestJoinButtonClicked()
{
	FJoinServerData TestPlayer;
	TestPlayer.Name = TEXT("Test player" + FString::FromInt(PlayerSlots.Num() + 1));
	TestPlayer.IPAddress = TEXT("123456");

	OnJoinPlayer(TestPlayer);
}

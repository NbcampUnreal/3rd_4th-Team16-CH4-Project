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
}

void UPOServerLobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPOServerLobbyWidget::OnReadyButtonClicked()
{
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnReadyStateChanged.Broadcast();
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
	
}

void UPOServerLobbyWidget::TestJoinButtonClicked()
{
	FJoinServerData TestPlayer;
	TestPlayer.Name = TEXT("Test player" + FString::FromInt(PlayerSlots.Num() + 1));
	TestPlayer.IPAddress = TEXT("123456");

	OnJoinPlayer(TestPlayer);
}

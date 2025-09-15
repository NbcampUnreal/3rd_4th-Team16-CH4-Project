// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ServerLobbyMenu/POServerLobbyWidget.h"

#include "Controllers/POServerLobbyPlayerController.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/ServerLobbyMenu/POServerLobbyPlayerElementWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Game/POLobbyPlayerState.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UPOServerLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReadyButton)
	{
		ReadyButton->OnClicked.AddDynamic(this, &UPOServerLobbyWidget::OnClickedReadyButton);
	}
	
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UPOServerLobbyWidget::OnClickedExitButton);
	}
	
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnReadyStateChanged.AddDynamic(this, &UPOServerLobbyWidget::OnReadyPlayer);
	}

	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerJoinLobby.AddDynamic(this, &UPOServerLobbyWidget::OnJoinPlayer);
	}
	
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerLeaveLobby.AddDynamic(this, &UPOServerLobbyWidget::OnExitPlayer);
	}

}

void UPOServerLobbyWidget::NativeDestruct()
{
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnReadyStateChanged.RemoveDynamic(this, &UPOServerLobbyWidget::OnReadyPlayer);
	}
	
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerJoinLobby.RemoveDynamic(this, &UPOServerLobbyWidget::OnJoinPlayer);
	}
	
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerLeaveLobby.RemoveDynamic(this, &UPOServerLobbyWidget::OnExitPlayer);
	}
	
	Super::NativeDestruct();
}

void UPOServerLobbyWidget::OnClickedReadyButton()
{
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerReady.Broadcast();
		UE_LOG(POLog, Log, TEXT("Ready Button Clicked"));
	}
}

void UPOServerLobbyWidget::OnClickedExitButton()
{
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwningPlayer()))
	{
		UE_LOG(POLog, Log, TEXT("Exit Button Clicked - Leaving server"));
		
		// 서버에서 로그아웃하고 메인 메뉴로 돌아감
		if (UWorld* World = GetWorld())
		{
			// 메인 메뉴 레벨로 이동
			UGameplayStatics::OpenLevel(World, TEXT("L_MainMenu"));
		}
	}
}

void UPOServerLobbyWidget::OnJoinPlayer(const FJoinServerData& InNewPlayer)
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

void UPOServerLobbyWidget::OnExitPlayer(const FJoinServerData& InExitPlayer)
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

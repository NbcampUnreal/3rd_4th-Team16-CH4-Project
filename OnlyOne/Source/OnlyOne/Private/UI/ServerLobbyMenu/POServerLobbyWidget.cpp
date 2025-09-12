// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ServerLobbyMenu/POServerLobbyWidget.h"

#include "Controllers/POServerLobbyPlayerController.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/ServerLobbyMenu/POServerLobbyPlayerElementWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"

void UPOServerLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
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
	// PlayerSlotClass가 설정되어 있는지 확인
	if (!PlayerSlotClass)
	{
		UE_LOG(POLog, Warning, TEXT("PlayerSlotClass is not set!"));
		return;
	}

	// 플레이어 이름을 키로 사용 (고유하다고 가정)
	FString PlayerKey = InNewPlayer.Name;

	// 이미 해당 플레이어의 슬롯이 있는지 확인
	if (PlayerSlots.Contains(PlayerKey))
	{
		UE_LOG(POLog, Warning, TEXT("Player slot already exists for key: %s"), *PlayerKey);
		return;
	}

	// PlayerSlotClass 인스턴스 생성
	UPOServerLobbyPlayerElementWidget* NewPlayerSlot = CreateWidget<UPOServerLobbyPlayerElementWidget>(this, PlayerSlotClass);
	
	if (NewPlayerSlot)
	{
		// PlayerSlots 맵에 저장
		PlayerSlots.Add(PlayerKey, NewPlayerSlot);
		
		// 필요한 경우 플레이어 데이터 설정 (NewPlayerSlot에 데이터 설정 함수가 있다면)
		// NewPlayerSlot->SetPlayerData(InNewPlayer);
		
		// 뷰포트에 추가 (필요한 경우)
		NewPlayerSlot->AddToViewport();
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

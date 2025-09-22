// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerStateList/POPlayerStateListWidget.h"
#include "UI/PlayerStateList/POPlayerStateElementWidget.h"
#include "Components/PanelWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"

void UPOPlayerStateListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AddPlayerStateEntry(TEXT("Player1"), true, 3 );
	AddPlayerStateEntry(TEXT("Player2"), false, 1 );
	AddPlayerStateEntry(TEXT("Player3"), true, 0 );
	
}

void UPOPlayerStateListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	ClearPlayerStateEntries();
}

UPOPlayerStateElementWidget* UPOPlayerStateListWidget::AddPlayerStateEntry(const FString& Nickname, bool bIsAlive, int32 KillCount)
{
	if (!PlayerList || !PlayerEntryClass)
	{
		return nullptr;
	}

	UPOPlayerStateElementWidget* Entry = CreateWidget<UPOPlayerStateElementWidget>(GetWorld(), PlayerEntryClass);
	if (!Entry)
	{
		return nullptr;
	}

	// 값 설정
	Entry->SetPlayerNickname(Nickname);
	Entry->SetIsAlive(bIsAlive);
	Entry->SetKillCount(KillCount);

	// 패널에 추가
	PlayerList->AddChild(Entry);
	return Entry;
}

void UPOPlayerStateListWidget::ClearPlayerStateEntries()
{
	if (!PlayerList)
	{
		return;
	}
	PlayerList->ClearChildren();
}

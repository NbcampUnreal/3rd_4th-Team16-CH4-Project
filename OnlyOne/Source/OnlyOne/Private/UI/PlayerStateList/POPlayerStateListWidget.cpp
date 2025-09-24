// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerStateList/POPlayerStateListWidget.h"
#include "UI/PlayerStateList/POPlayerStateElementWidget.h"
#include "Components/PanelWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Controllers/POPlayerController.h"

void UPOPlayerStateListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APOPlayerController* PC = GetOwningPlayer<APOPlayerController>())
	{
		if (!PC->OnSetPlayerStateEntry.IsBoundToObject(this))
		{
			PC->OnSetPlayerStateEntry.AddUObject(this, &UPOPlayerStateListWidget::SetPlayerStateEntry);
		}
	}

	
	SetPlayerStateEntry(TEXT("Player1"), true, 3 );
	SetPlayerStateEntry(TEXT("Player2"), false, 1 );
	SetPlayerStateEntry(TEXT("Player3"), true, 0 );
	
}

void UPOPlayerStateListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (APOPlayerController* PC = GetOwningPlayer<APOPlayerController>())
	{
		PC->OnSetPlayerStateEntry.RemoveAll(this);
	}

	ClearPlayerStateEntries();
	PlayerEntries.Empty();
}

void UPOPlayerStateListWidget::SetPlayerStateEntry(const FString& Nickname, bool bIsAlive, int32 KillCount)
{
	if (!PlayerList || !PlayerEntryClass)
	{
		return;
	}

	if (!PlayerEntries.Contains(Nickname))
	{
		UPOPlayerStateElementWidget* Entry = CreateWidget<UPOPlayerStateElementWidget>(GetWorld(), PlayerEntryClass);
		if (!Entry)
		{
			return;
		}

		Entry->SetPlayerNickname(Nickname);
		Entry->SetIsAlive(bIsAlive);
		Entry->SetKillCount(KillCount);

		PlayerList->AddChild(Entry);
		PlayerEntries.Add(Nickname, Entry);
	}
	else
	{
		if (UPOPlayerStateElementWidget* Entry = PlayerEntries[Nickname])
		{
			Entry->SetIsAlive(bIsAlive);
			Entry->SetKillCount(KillCount);
		}
	}
}

void UPOPlayerStateListWidget::ClearPlayerStateEntries()
{
	if (!PlayerList)
	{
		return;
	}
	PlayerList->ClearChildren();
	PlayerEntries.Empty();
}

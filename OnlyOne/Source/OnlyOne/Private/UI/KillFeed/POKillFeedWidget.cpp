// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KillFeed/POKillFeedWidget.h"

#include "Components/ScrollBox.h"
#include "UI/KillFeed/POKillFeedElementWidget.h"

void UPOKillFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// TODO: 위젯 초기화 코드 작성
	// 테스트용 더미 데이터 시간 마다 추가
	GetWorld()->GetTimerManager().SetTimer(
		TestAddKillFeedEntry,
		[this]()
		{
			AddKillFeedEntry(TEXT("Killer"), TEXT("Victim"));
		},
		4.0f,
		true
	);
}

void UPOKillFeedWidget::NativeDestruct()
{
	Super::NativeDestruct();
	// TODO: 위젯 해제 코드 작성
}

void UPOKillFeedWidget::AddKillFeedEntry(const FString& KillerName, const FString& VictimName)
{
	if (KillFeedEntryClass && KillFeedList)
	{
		if (UPOKillFeedElementWidget* NewEntry = CreateWidget<UPOKillFeedElementWidget>(this, KillFeedEntryClass))
		{
			NewEntry->SetKillFeedTexts(FText::FromString(KillerName), FText::FromString(VictimName));
			KillFeedList->AddChild(NewEntry);

			GetWorld()->GetTimerManager().SetTimer(
				KillFeedTimer,
				[NewEntry]()
				{
					if (NewEntry)
					{
						NewEntry->RemoveKillFeedEntry();
					}
				},
				KillFeedDuration,
				false
			);
		}
	}
}
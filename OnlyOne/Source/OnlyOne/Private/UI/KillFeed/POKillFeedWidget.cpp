// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KillFeed/POKillFeedWidget.h"

#include "Components/ScrollBox.h"
#include "UI/KillFeed/POKillFeedElementWidget.h"
#include "Game/POStageGameState.h"

void UPOKillFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// APOStageGameState의 OnKillEvent에 바인딩
	if (UWorld* World = GetWorld())
	{
		if (APOStageGameState* StageGameState = World->GetGameState<APOStageGameState>())
		{
			BoundGameState = StageGameState;
			StageGameState->OnKillEvent.AddDynamic(this, &UPOKillFeedWidget::AddKillFeedEntry);
			UE_LOG(LogTemp, Log, TEXT("KillFeedWidget: Bound to OnKillEvent"));
		}
	}
}

void UPOKillFeedWidget::NativeDestruct()
{
	// 바인딩 해제
	if (BoundGameState)
	{
		BoundGameState->OnKillEvent.RemoveDynamic(this, &UPOKillFeedWidget::AddKillFeedEntry);
		BoundGameState = nullptr;
	}
	
	Super::NativeDestruct();
}

void UPOKillFeedWidget::AddKillFeedEntry(const FString& KillerName, const FString& VictimName)
{
	if (KillFeedEntryClass && KillFeedList)
	{
		if (UPOKillFeedElementWidget* NewEntry = CreateWidget<UPOKillFeedElementWidget>(this, KillFeedEntryClass))
		{
			FTimerHandle KillFeedTimer;
			
			NewEntry->SetKillFeedTexts(FText::FromString(KillerName), FText::FromString(VictimName));
			KillFeedList->AddChild(NewEntry);

			GetWorld()->GetTimerManager().SetTimer(
				KillFeedTimer,
				[this, NewEntry]()
				{
					if (NewEntry)
					{
						KillFeedList->RemoveChild(NewEntry);
						NewEntry->RemoveKillFeedEntry();
					}
				},
				KillFeedDuration,
				false
			);
		}
	}
}
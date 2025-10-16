// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StageTimer/POStageTimerWidget.h"
#include "Components/TextBlock.h"
#include "Game/POStageGameState.h"
#include "Engine/World.h"

void UPOStageTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StageGameState = GetWorld() ? GetWorld()->GetGameState<APOStageGameState>() : nullptr;
	if (StageGameState.IsValid())
	{
		// 초기 값 세팅
		HandleStageTimeUpdated(StageGameState->GetStageRemainingSeconds());
		// 델리게이트 등록
		StageTimeUpdatedHandle = StageGameState->OnStageTimeUpdated.AddUObject(this, &UPOStageTimerWidget::HandleStageTimeUpdated);
	}
}

void UPOStageTimerWidget::NativeDestruct()
{
	if (StageGameState.IsValid() && StageTimeUpdatedHandle.IsValid())
	{
		StageGameState->OnStageTimeUpdated.Remove(StageTimeUpdatedHandle);
	}
	Super::NativeDestruct();
}

void UPOStageTimerWidget::HandleStageTimeUpdated(int32 InRemainingSeconds)
{
	if (!StageTimeText) return;
	if (InRemainingSeconds < 0)
	{
		InRemainingSeconds = 0; // 안전 처리
	}
	StageTimeText->SetText(FormatTimeMMSS(InRemainingSeconds));
}

FText UPOStageTimerWidget::FormatTimeMMSS(int32 InTotalSeconds) const
{
	const int32 Minutes = InTotalSeconds / 60;
	const int32 Seconds = InTotalSeconds % 60;
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}

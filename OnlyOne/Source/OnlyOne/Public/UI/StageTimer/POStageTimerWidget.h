// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POStageTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOStageTimerWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	/* UI 바인드: Stage 남은 시간 표시 TextBlock (UMG 디자이너에서 동일 이름 위젯 필요) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> StageTimeText;

private:
	TWeakObjectPtr<class APOStageGameState> StageGameState;
	FDelegateHandle StageTimeUpdatedHandle;

	void HandleStageTimeUpdated(int32 InRemainingSeconds);
	FText FormatTimeMMSS(int32 InTotalSeconds) const;
};

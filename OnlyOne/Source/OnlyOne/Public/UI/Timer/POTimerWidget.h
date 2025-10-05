// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POTimerWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOTimerWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Timer")
	void SetTimerText(int32 NewTimeSeconds);

	void SetTimerText(const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Timer")
	UTextBlock* TimerText = nullptr;

private:
	bool bTimerBound = false;
};

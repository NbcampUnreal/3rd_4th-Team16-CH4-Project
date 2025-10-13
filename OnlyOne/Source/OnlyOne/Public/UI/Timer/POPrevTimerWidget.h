// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POTimerWidget.h"
#include "POPrevTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPrevTimerWidget : public UPOTimerWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void SetTimerText(int32 NewTimeSeconds) override;
};

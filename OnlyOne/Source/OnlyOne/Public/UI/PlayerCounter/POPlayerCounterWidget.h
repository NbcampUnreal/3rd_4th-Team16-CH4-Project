// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "UI/Common/POOneTextWidget.h"
#include "POPlayerCounterWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPlayerCounterWidget : public UPOOneTextWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	FTimerHandle AliveUiTickHandle;
	int32 LastShownAlive = INT32_MIN;

	UFUNCTION()
	void TickAliveUi(); 
};

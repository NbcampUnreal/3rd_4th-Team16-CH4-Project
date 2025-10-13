// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POTimerWidget.h"
#include "POGameStartTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOGameStartTimerWidget : public UPOTimerWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
};

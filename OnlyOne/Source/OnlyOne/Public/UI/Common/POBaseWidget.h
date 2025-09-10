// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "POBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetInitialized();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POBaseWidget.h"
#include "POBaseWindow.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOBaseWindow : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UFUNCTION()
	void OnExitButtonClicked();
};

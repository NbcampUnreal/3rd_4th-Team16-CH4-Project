// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POBaseWidget.h"
#include "POSelectBoxWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOSelectBoxWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> YesButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NoButton;

	UFUNCTION()
	virtual void OnYesButtonClicked();

	UFUNCTION()
	virtual void OnNoButtonClicked();
};

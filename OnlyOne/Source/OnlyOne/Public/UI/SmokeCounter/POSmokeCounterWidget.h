// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POSmokeCounterWidget.generated.h"

UCLASS()
class ONLYONE_API UPOSmokeCounterWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SmokeCounter")
	void SetSmokeCount(int32 NewCount);

	UFUNCTION(BlueprintCallable, Category="SmokeCounter")
	void SetSmokeText(const FText& InText);

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> SmokeText; 
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POBaseWidget.h"
#include "POOneTextWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOOneTextWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="UI")
	void SetTextBox(const FText& InText);
	void SetTextBox(int32 NewCount);

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> MainText; 
};

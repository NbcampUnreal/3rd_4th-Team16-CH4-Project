// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "UI/Common/POBaseWidget.h"
#include "POKillFeedElementWidget.generated.h"

/**
 * 
 */

class UPOKillFeedWidget;

UCLASS()
class ONLYONE_API UPOKillFeedElementWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="KillFeed")
	void SetKillFeedTexts(const FText& InKillerText, const FText& InVictimText);

	UFUNCTION(Category = "KillFeed")
	void RemoveKillFeedEntry();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> KillerText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> VictimText;

};


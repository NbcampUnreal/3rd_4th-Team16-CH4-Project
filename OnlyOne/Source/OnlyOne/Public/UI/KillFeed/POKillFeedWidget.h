// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POKillFeedWidget.generated.h"

class UPOKillFeedElementWidget;
class UScrollBox;
class UTextBlock;
/**
 * 
 */

UCLASS()
class ONLYONE_API UPOKillFeedWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AddKillFeedEntry(const FString& KillerName, const FString& VictimName);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> KillFeedList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStateList")
	TSubclassOf<UPOKillFeedElementWidget> KillFeedEntryClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KillFeed")
	float KillFeedDuration = 5.0f;

private:
	FTimerHandle KillFeedTimer;

	/*테스트 용 코드 */
	FTimerHandle TestAddKillFeedEntry;
};

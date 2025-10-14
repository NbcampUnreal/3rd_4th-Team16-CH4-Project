// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POKillFeedWidget.generated.h"

class UPOKillFeedElementWidget;
class UScrollBox;
class UTextBlock;
class APOStageGameState;
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

	UFUNCTION()
	void AddKillFeedEntry(const FString& KillerName, const FString& VictimName);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStateList")
	TSubclassOf<UPOKillFeedElementWidget> KillFeedEntryClass;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> KillFeedList;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KillFeed")
	float KillFeedDuration = 5.0f;

private:
	/*테스트 용 코드 */
	FTimerHandle TestAddKillFeedEntry;

	/** 바인딩된 GameState 참조 (언바인딩에 사용) */
	UPROPERTY()
	TObjectPtr<APOStageGameState> BoundGameState;
};

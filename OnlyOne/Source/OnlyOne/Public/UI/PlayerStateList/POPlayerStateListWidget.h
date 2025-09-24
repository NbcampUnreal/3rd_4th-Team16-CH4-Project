// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POPlayerStateListWidget.generated.h"

class UScrollBox;
class UPanelWidget;
class UPOPlayerStateElementWidget;

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPlayerStateListWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetPlayerStateEntry(const FString& Nickname, bool bIsAlive, int32 KillCount);
	void ClearPlayerStateEntries();

protected:
	// TODO: UMG 디자이너에서 동일한 이름의 패널(VerticalBox/ScrollBox 내 PanelWidget 등)을 바인딩하세요.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> PlayerList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStateList")
	TSubclassOf<UPOPlayerStateElementWidget> PlayerEntryClass;

private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UPOPlayerStateElementWidget>> PlayerEntries;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POPlayerStateElementWidget.generated.h"

// Forward declaration to avoid heavy includes in header
class UTextBlock;

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPlayerStateElementWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetPlayerNickname(const FString& InNickname);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetIsAlive(bool bInAlive);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetKillCount(int32 InKillCount);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NicknameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AliveStateText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KillCountText;

private:
	// NOTE: 값이 먼저 설정되고 위젯이 아직 생성되지 않았을 수 있으므로 캐싱합니다
	FString CachedNickname;
	bool bCachedAlive = false;
	int32 CachedKillCount = 0;

	void ApplyNickname();
	void ApplyAliveState();
	void ApplyKillCount();
};

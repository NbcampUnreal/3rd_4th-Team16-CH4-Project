// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

class UPOPlayerStateListWidget;

#include "POUITestPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APOUITestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	// KillFeed 위젯 블루프린트 클래스 지정용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI|KillFeed")
	TSubclassOf<class UPOKillFeedWidget> KillFeedWidgetClass;

	// 생성된 KillFeed 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<class UPOKillFeedWidget> KillFeedWidget;
};

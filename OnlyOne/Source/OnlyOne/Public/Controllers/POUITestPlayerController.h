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
	virtual void SetupInputComponent() override;

protected:
	// Tab 키 동작
	void OnTabPressed();
	void OnTabReleased();

	// 위젯 생성/표시/숨김
	void EnsureListWidgetCreated();
	void ShowListWidget();
	void HideListWidget();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPOPlayerStateListWidget> PlayerStateListWidgetClass;

	UPROPERTY(Transient)
	UPOPlayerStateListWidget* PlayerStateListWidget;
};

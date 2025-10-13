// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POHowToPlayWidget.generated.h"

class UPOBaseWindow;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOHowToPlayWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOBaseWindow> WindowUI;

private:
	UFUNCTION()
	void OnCloseWindow();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POMainMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOMainMenuWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* JoinServerButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* SettingsButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* QuitButton;

private:
    UFUNCTION()
    void OnJoinServerClicked();
    
    UFUNCTION()
    void OnSettingsClicked();
    
    UFUNCTION()
    void OnQuitClicked();
};

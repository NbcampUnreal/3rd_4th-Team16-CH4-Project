// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POMainMenuWidget.generated.h"

class UPOCustomButton;
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
    virtual void NativeDestruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPOCustomButton> JoinServerButton;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPOCustomButton> SettingsButton;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPOCustomButton> QuitButton;

private:
    UFUNCTION()
    void OnJoinServerClicked(UPOCustomButton* ClickedButton);
    
    UFUNCTION()
    void OnSettingsClicked(UPOCustomButton* ClickedButton);
    
    UFUNCTION()
    void OnQuitClicked(UPOCustomButton* ClickedButton);
};

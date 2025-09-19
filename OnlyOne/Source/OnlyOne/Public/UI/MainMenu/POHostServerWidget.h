// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POHostServerWidget.generated.h"

class UEditableTextBox;
class UPOBaseWindow;
class UButton;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOHostServerWidget : public UPOBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOBaseWindow> WindowUI;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> UsernameTextBox;
	
private:
	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnCloseWindow();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "UI/Common/POCustomButton.h"
#include "POJoinServerWidget.generated.h"

class UPOBaseWindow;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOJoinServerWidget : public UPOBaseWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOBaseWindow> WindowUI;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> UsernameTextBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ServerAddressTextBox;
	
private:
	UFUNCTION()
	void OnJoinButtonClicked();

	UFUNCTION()
	void OnCloseWindow();
};

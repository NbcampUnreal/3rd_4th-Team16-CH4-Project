// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "POCustomButton.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;
// 버튼 클릭 이벤트를 위한 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFadeButtonClicked, class UPOCustomButton*, ClickedButton);

UCLASS()
class ONLYONE_API UPOCustomButton : public UUserWidget
{
	GENERATED_BODY()
public:
	UPOCustomButton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Button Events")
	FOnFadeButtonClicked OnCustomButtonClicked;
    
	UFUNCTION(BlueprintCallable, Category = "Button")
	void SetButtonEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Button")
	void SetButtonText(const FText& InText);

protected:
	virtual void NativeConstruct() override;
    
	UPROPERTY(meta = (BindWidget))
	UButton* MainButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ButtonText;

	UFUNCTION()
	void OnButtonClicked();
    
private:
	bool bIsEnabled;
};

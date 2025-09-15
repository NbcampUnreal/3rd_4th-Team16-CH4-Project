// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POCustomButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UPOCustomButton::UPOCustomButton(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
                                                                               , bIsEnabled(true)
{
}

void UPOCustomButton::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (MainButton)
	{
		MainButton->OnClicked.AddDynamic(this, &UPOCustomButton::OnButtonClicked);
	}
}

void UPOCustomButton::NativeDestruct()
{
	OnCustomButtonClicked.Clear();
	
	Super::NativeDestruct();
}

void UPOCustomButton::SetButtonEnabled(bool bEnabled)
{
	bIsEnabled = bEnabled;
    
	if (MainButton)
	{
		MainButton->SetIsEnabled(bEnabled);
	}
}

void UPOCustomButton::SetButtonText(const FText& InText)
{
	if (ButtonText)
	{
		ButtonText->SetText(InText);
	}
}

void UPOCustomButton::OnButtonClicked()
{
	if (!bIsEnabled)
	{
		return;
	}
        
	if (OnCustomButtonClicked.IsBound())
	{
		OnCustomButtonClicked.Broadcast(this);
	}
}

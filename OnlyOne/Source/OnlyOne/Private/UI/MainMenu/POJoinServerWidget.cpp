// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/POJoinServerWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/Common/POBaseWindow.h"

void UPOJoinServerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WindowUI)
	{
		WindowUI->OnCloseWindow.AddDynamic(this, &UPOJoinServerWidget::RemoveFromParent);
	}
	
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UPOJoinServerWidget::OnJoinButtonClicked);
	}
}

void UPOJoinServerWidget::BeginDestroy()
{
	Super::BeginDestroy();
}

void UPOJoinServerWidget::OnJoinButtonClicked()
{
	FString IP = ServerAddressTextBox->GetText().ToString();
	FString Username = UsernameTextBox->GetText().ToString();
	UE_LOG(POLog, Log, TEXT("IP: %s, Username: %s"), *IP, *Username);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/POJoinServerWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "Controllers/Components/POUIStackingComponent.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/Common/POBaseWindow.h"

void UPOJoinServerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WindowUI && !WindowUI->OnCloseWindow.IsBound())
	{
		WindowUI->OnCloseWindow.AddDynamic(this, &UPOJoinServerWidget::OnCloseWindow);
	}
	
	if (JoinButton && !JoinButton->OnClicked.IsBound())
	{
		JoinButton->OnClicked.AddDynamic(this, &UPOJoinServerWidget::OnJoinButtonClicked);
	}
}

void UPOJoinServerWidget::NativeDestruct()
{
	if (WindowUI && WindowUI->OnCloseWindow.IsBound())
	{
		WindowUI->OnCloseWindow.RemoveDynamic(this, &UPOJoinServerWidget::OnCloseWindow);
	}

	if (JoinButton && JoinButton->OnClicked.IsBound())
	{
		JoinButton->OnClicked.RemoveDynamic(this, &UPOJoinServerWidget::OnJoinButtonClicked);
	}
	
	Super::NativeDestruct();
}

void UPOJoinServerWidget::OnJoinButtonClicked()
{
	FString IP = ServerAddressTextBox->GetText().ToString();
	FString Username = UsernameTextBox->GetText().ToString();
	FJoinServerData JoinServerData;
	JoinServerData.IPAddress = IP;
	JoinServerData.Name = Username;
	
	if (APOMainMenuPlayerController* PC = Cast<APOMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->OnJoinServer(JoinServerData);
		UE_LOG(POLog, Log, TEXT("IP: %s, Username: %s"), *IP, *Username);
	}
}

void UPOJoinServerWidget::OnCloseWindow()
{
	if (IPOUIStackingInterface* UIStackingInterface = Cast<IPOUIStackingInterface>(GetOwningPlayer()))
	{
		UIStackingInterface->GetUIStackingComponent()->PopWidget();
	}
}

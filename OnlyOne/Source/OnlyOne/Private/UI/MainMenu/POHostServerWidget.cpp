// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/POHostServerWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "Controllers/Components/POUIStackingComonent.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/Common/POBaseWindow.h"

void UPOHostServerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WindowUI && !WindowUI->OnCloseWindow.IsAlreadyBound(this, &UPOHostServerWidget::OnCloseWindow))
	{
		WindowUI->OnCloseWindow.AddDynamic(this, &UPOHostServerWidget::OnCloseWindow);
	}
	
	if (HostButton && !HostButton->OnClicked.IsAlreadyBound(this, &UPOHostServerWidget::OnHostButtonClicked))
	{
		HostButton->OnClicked.AddDynamic(this, &UPOHostServerWidget::OnHostButtonClicked);
	}
}

void UPOHostServerWidget::NativeDestruct()
{
	if (WindowUI && WindowUI->OnCloseWindow.IsAlreadyBound(this, &UPOHostServerWidget::OnCloseWindow))
	{
		WindowUI->OnCloseWindow.RemoveDynamic(this, &UPOHostServerWidget::OnCloseWindow);
	}
	if (HostButton && HostButton->OnClicked.IsAlreadyBound(this, &UPOHostServerWidget::OnHostButtonClicked))
	{
		HostButton->OnClicked.RemoveDynamic(this, &UPOHostServerWidget::OnHostButtonClicked);
	}
	
	Super::NativeDestruct();
}

void UPOHostServerWidget::OnHostButtonClicked()
{
	FString Username = UsernameTextBox->GetText().ToString();
	FJoinServerData HostServerData;
	HostServerData.Name = Username;
	
	if (APOMainMenuPlayerController* PC = Cast<APOMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->OnHostServer(HostServerData);
		UE_LOG(POLog, Log, TEXT("Username: %s"), *Username);
	}
}

void UPOHostServerWidget::OnCloseWindow()
{
	if (IPOUIStackingInterface* UIStackingInterface = Cast<IPOUIStackingInterface>(GetOwningPlayer()))
	{
		UIStackingInterface->GetUIStackingComponent()->PopWidget();
	}
}

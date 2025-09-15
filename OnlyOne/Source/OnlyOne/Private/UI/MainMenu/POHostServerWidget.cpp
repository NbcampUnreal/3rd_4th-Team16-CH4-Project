// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/POHostServerWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/Common/POBaseWindow.h"

void UPOHostServerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WindowUI && !WindowUI->OnCloseWindow.IsBound())
	{
		WindowUI->OnCloseWindow.AddDynamic(this, &UPOHostServerWidget::OnCloseWindow);
	}
	
	if (HostButton && !HostButton->OnClicked.IsBound())
	{
		HostButton->OnClicked.AddDynamic(this, &UPOHostServerWidget::OnHostButtonClicked);
	}
}

void UPOHostServerWidget::NativeDestruct()
{
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
	SetVisibility(ESlateVisibility::Collapsed);
}

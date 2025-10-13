// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HowToPlay/POHowToPlayWidget.h"

#include "Controllers/Components/POUIStackingComponent.h"
#include "Controllers/Interfaces/POUIStackingInterface.h"
#include "UI/Common/POBaseWindow.h"
#include "UI/MainMenu/POHostServerWidget.h"

void UPOHowToPlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WindowUI && !WindowUI->OnCloseWindow.IsAlreadyBound(this, &UPOHowToPlayWidget::OnCloseWindow))
	{
		WindowUI->OnCloseWindow.AddDynamic(this, &UPOHowToPlayWidget::OnCloseWindow);
	}
}

void UPOHowToPlayWidget::NativeDestruct()
{
	if (WindowUI && WindowUI->OnCloseWindow.IsAlreadyBound(this, &UPOHowToPlayWidget::OnCloseWindow))
	{
		WindowUI->OnCloseWindow.RemoveDynamic(this, &UPOHowToPlayWidget::OnCloseWindow);
	}
	
	Super::NativeDestruct();
}

void UPOHowToPlayWidget::OnCloseWindow()
{
	if (IPOUIStackingInterface* UIStackingInterface = Cast<IPOUIStackingInterface>(GetOwningPlayer()))
	{
		UIStackingInterface->GetUIStackingComponent()->PopWidget();
	}
}

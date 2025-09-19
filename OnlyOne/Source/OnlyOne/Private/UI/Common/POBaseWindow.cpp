// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POBaseWindow.h"
#include "Components/Button.h"

void UPOBaseWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ExitButton && !ExitButton->OnClicked.IsAlreadyBound(this, &UPOBaseWindow::OnExitButtonClicked))
	{
		ExitButton->OnClicked.AddDynamic(this, &UPOBaseWindow::OnExitButtonClicked);
	}
}

void UPOBaseWindow::NativeDestruct()
{
	if (ExitButton && ExitButton->OnClicked.IsAlreadyBound(this, &UPOBaseWindow::OnExitButtonClicked))
	{
		ExitButton->OnClicked.RemoveDynamic(this, &UPOBaseWindow::OnExitButtonClicked);
	}
	
	OnCloseWindow.Clear();
	
	Super::NativeDestruct();
}

void UPOBaseWindow::OnExitButtonClicked()
{
	OnCloseWindow.Broadcast();
}

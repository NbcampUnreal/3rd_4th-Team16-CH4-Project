// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POBaseWindow.h"
#include "Components/Button.h"

void UPOBaseWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UPOBaseWindow::OnExitButtonClicked);
	}
}

void UPOBaseWindow::BeginDestroy()
{
	OnCloseWindow.Clear();
	//ExitButton->OnClicked.Clear();
	
	Super::BeginDestroy();
}

void UPOBaseWindow::OnExitButtonClicked()
{
	OnCloseWindow.Broadcast();
}

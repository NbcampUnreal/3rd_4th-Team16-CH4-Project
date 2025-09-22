// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/POUITestPlayerController.h"
#include "UI/PlayerStateList/POPlayerStateListWidget.h"
#include "Blueprint/UserWidget.h"
#include "InputCoreTypes.h"

void APOUITestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &APOUITestPlayerController::OnTabPressed);
	InputComponent->BindKey(EKeys::Tab, IE_Released, this, &APOUITestPlayerController::OnTabReleased);
}

void APOUITestPlayerController::OnTabPressed()
{
	ShowListWidget();
}

void APOUITestPlayerController::OnTabReleased()
{
	HideListWidget();
}

void APOUITestPlayerController::EnsureListWidgetCreated()
{
	if (PlayerStateListWidget == nullptr)
	{
		if (!PlayerStateListWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerStateListWidgetClass is not set on %s"), *GetName());
			return;
		}

		PlayerStateListWidget = CreateWidget<UPOPlayerStateListWidget>(this, PlayerStateListWidgetClass);
		if (!PlayerStateListWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create PlayerStateListWidget"));
		}
	}
}

void APOUITestPlayerController::ShowListWidget()
{
	EnsureListWidgetCreated();
	if (PlayerStateListWidget)
	{
		if (!PlayerStateListWidget->IsInViewport())
		{
			PlayerStateListWidget->AddToViewport();
		}
		PlayerStateListWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void APOUITestPlayerController::HideListWidget()
{
	if (PlayerStateListWidget)
	{
		PlayerStateListWidget->RemoveFromParent();
	}
}

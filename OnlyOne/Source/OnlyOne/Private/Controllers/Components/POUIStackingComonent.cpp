// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/Components/POUIStackingComonent.h"

#include "Blueprint/UserWidget.h"

UPOUIStackingComonent::UPOUIStackingComonent()
{
	
}

void UPOUIStackingComonent::PushWidget(UUserWidget* Widget)
{
	if (!Widget) 
	{
		return;
	}

	if (!UIStack.IsEmpty())
	{
		UIStack.Last()->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (UIStack.Num() <= 0)
	{
		if (APlayerController* PC = GetOwner<APlayerController>())
		{
			PC->SetInputMode(FInputModeUIOnly());
			PC->bShowMouseCursor = true;
		}
	}

	UIStack.Add(Widget);
	Widget->AddToViewport();
	Widget->SetVisibility(ESlateVisibility::Visible);

}

void UPOUIStackingComonent::PopWidget()
{
	if (UIStack.IsEmpty()) return;

	UUserWidget* TopWidget = UIStack.Pop();
	TopWidget->RemoveFromParent();
	
	if (!UIStack.IsEmpty()) // 이전 위젯 다시 표시
	{
		UIStack.Last()->SetVisibility(ESlateVisibility::Visible);
	}
	else if (UIStack.IsEmpty())
	{
		if (APlayerController* PC = GetOwner<APlayerController>())
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
}

void UPOUIStackingComonent::PopToWidget(UUserWidget* Widget)
{
	while (!UIStack.IsEmpty() && UIStack.Last() != Widget)
	{
		PopWidget();
	}
}

void UPOUIStackingComonent::ClearStack()
{
	while (!UIStack.IsEmpty())
	{
		PopWidget();
	}
}

void UPOUIStackingComonent::BeginPlay()
{
	Super::BeginPlay();

	
}
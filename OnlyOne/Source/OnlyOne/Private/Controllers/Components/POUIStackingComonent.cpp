// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/Components/POUIStackingComonent.h"

#include "Blueprint/UserWidget.h"

UPOUIStackingComonent::UPOUIStackingComonent()
{
	bIsInputModeUIOnly = false;
	UIStack.Add(nullptr);
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

	UIStack.Add(Widget);
	Widget->AddToViewport();
	Widget->SetVisibility(ESlateVisibility::Visible);
}

void UPOUIStackingComonent::PopWidget()
{
	if (UIStack.IsEmpty() || UIStack.Num() <= 1) return;

	UUserWidget* TopWidget = UIStack.Pop();
	TopWidget->RemoveFromParent();
	
	if (!UIStack.IsEmpty()) // 이전 위젯 다시 표시
	{
		UIStack.Last()->SetVisibility(ESlateVisibility::Visible);
	}
	else if (UIStack.Num() <= 1) // 스택이 비었고 UI 전용 모드인 경우 게임 모드로 전환
	{
		if (APlayerController* PC = GetOwner<APlayerController>())
		{
			if (bIsInputModeUIOnly)
			{
				PC->SetInputMode(FInputModeUIOnly());
				PC->bShowMouseCursor = true;
			}
			else
			{
				PC->SetInputMode(FInputModeGameOnly());
				PC->bShowMouseCursor = false;
			}
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

void UPOUIStackingComonent::SetDefaultWidget(UUserWidget* Widget, bool bInputModeUIOnly)
{
	UIStack[0] = Widget;
	bIsInputModeUIOnly = bInputModeUIOnly;
	
	if (bInputModeUIOnly)
	{
		if (APlayerController* PC = GetOwner<APlayerController>())
		{
			PC->SetInputMode(FInputModeUIOnly());
			PC->bShowMouseCursor = true;
		}
	}
	
	Widget->AddToViewport();
	Widget->SetVisibility(ESlateVisibility::Visible);
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
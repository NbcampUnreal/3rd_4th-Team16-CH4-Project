// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/Components/POUIStackingComponent.h"

#include "Blueprint/UserWidget.h"

UPOUIStackingComponent::UPOUIStackingComponent()
{
	bIsInputModeUIOnly = false;
	UIStack.Add(nullptr);
}

void UPOUIStackingComponent::PushWidget(UUserWidget* Widget)
{
	if (!Widget) 
	{
		return;
	}

	if (!UIStack.IsEmpty())
	{
		if (UUserWidget* Last = UIStack.Last())
		{
			Last->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UIStack.Add(Widget);
	Widget->AddToViewport();
	Widget->SetVisibility(ESlateVisibility::Visible);
	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(Widget->TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UPOUIStackingComponent::PopWidget()
{
	if (UIStack.IsEmpty() || UIStack.Num() <= 1) return;

	UUserWidget* TopWidget = UIStack.Pop();
	TopWidget->RemoveFromParent();
	
	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		if (UIStack.Num() > 1)
		{
			// 이전 위젯 다시 표시 (여전히 오버레이 UI가 존재)
			UIStack.Last()->SetVisibility(ESlateVisibility::Visible);
			// 상단 위젯에 포커스 전달
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(UIStack.Last()->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
		else // UI 스택에 기본 위젯만 남은 경우 입력 모드 복구
		{
			// 기본 위젯 다시 표시
			if (UIStack.Last())
			{
				UIStack.Last()->SetVisibility(ESlateVisibility::Visible);
			}

			if (bIsInputModeUIOnly)
			{
				FInputModeUIOnly InputMode;
				if (UIStack.Last())
				{
					InputMode.SetWidgetToFocus(UIStack.Last()->TakeWidget());
				}
				PC->SetInputMode(InputMode);
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

void UPOUIStackingComponent::PopToWidget(UUserWidget* Widget)
{
	while (!UIStack.IsEmpty() && UIStack.Last() != Widget)
	{
		PopWidget();
	}
}

void UPOUIStackingComponent::SetDefaultWidget(UUserWidget* Widget, bool bInputModeUIOnly)
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

void UPOUIStackingComponent::ClearStack()
{
	UIStack.Empty();
}

void UPOUIStackingComponent::BeginPlay()
{
	Super::BeginPlay();

	
}
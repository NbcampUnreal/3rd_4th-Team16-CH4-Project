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
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(Widget->TakeWidget());
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void UPOUIStackingComponent::PopWidget()
{
    if (UIStack.IsEmpty() || UIStack.Num() <= 1) return;

    UUserWidget* TopWidget = UIStack.Pop();
    if (TopWidget)
    {
        //TopWidget->RemoveFromParent();
        TopWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

    // 새 최상단 위젯 결정 (nullptr 안전)
    UUserWidget* NewTop = nullptr;
    for (int32 i = UIStack.Num() - 1; i >= 0; --i)
    {
        if (UIStack[i])
        {
            NewTop = UIStack[i];
            break;
        }
    }

    if (NewTop)
    {
        if (!NewTop->IsInViewport())
        {
            NewTop->AddToViewport();
        }
        NewTop->SetVisibility(ESlateVisibility::Visible);
    }

    if (APlayerController* PC = GetOwner<APlayerController>())
    {
        if (NewTop)
        {
            // 디폴트만 남았고 UIOnly를 원하면 UIOnly로, 그 외에는 GameAndUI로 포커스 복구
            if (UIStack.Num() == 1 && bIsInputModeUIOnly)
            {
                FInputModeGameAndUI InputMode;
                InputMode.SetWidgetToFocus(NewTop->TakeWidget());
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
            else
            {
                FInputModeGameOnly InputMode;
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = false;
            }
        }
        else
        {
            // 남은 위젯이 전혀 없는 경우에 대비한 폴백 (정상 경로에선 발생하지 않음)
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

void UPOUIStackingComponent::PopToWidget(UUserWidget* Widget)
{
    if (UIStack.IsEmpty()) return;

    // 대상 위젯이 스택에 있는지 먼저 확인 (없으면 무한 루프 방지)
    int32 TargetIndex = INDEX_NONE;
    for (int32 i = 0; i < UIStack.Num(); ++i)
    {
        if (UIStack[i] == Widget)
        {
            TargetIndex = i;
            break;
        }
    }

    if (TargetIndex == INDEX_NONE)
    {
        return; // 대상이 없으면 아무 것도 하지 않음
    }

    // 최상단이 대상이 될 때까지 Pop
    while (UIStack.Num() > TargetIndex + 1)
    {
        PopWidget();
    }
}

void UPOUIStackingComponent::SetDefaultWidget(UUserWidget* Widget, bool bInputModeUIOnly)
{
    if (UIStack.Num() == 0)
    {
        // 0번 인덱스를 보장
        UIStack.Add(nullptr);
    }

    UIStack[0] = Widget;
    bIsInputModeUIOnly = bInputModeUIOnly;

    if (Widget)
    {
        if (!Widget->IsInViewport())
        {
            Widget->AddToViewport();
        }

        // 현재 최상단이 디폴트가 아니면 디폴트는 접어둔다
        if (UIStack.Last() == Widget)
        {
            Widget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            Widget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (APlayerController* PC = GetOwner<APlayerController>())
    {
        // 디폴트가 최상단일 때만 입력 모드를 반영
        if (UIStack.Last() == Widget)
        {
            if (bInputModeUIOnly)
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(Widget ? Widget->TakeWidget() : TSharedPtr<SWidget>());
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

void UPOUIStackingComponent::ClearStack()
{
    UIStack.Empty();
}

void UPOUIStackingComponent::BeginPlay()
{
    Super::BeginPlay();

    
}
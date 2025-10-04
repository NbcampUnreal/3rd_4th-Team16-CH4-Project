#include "UI/SettingMenu/POSettingWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Controllers/Components/POUIStackingComponent.h"
#include "Controllers/Interfaces/POUIStackingInterface.h"
#include "Engine/World.h"
#include "UI/Common/POBaseWindow.h"
#include "UI/MainMenu/POJoinServerWidget.h"
#include "Game/POGameInstance.h"

void UPOSettingWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (WindowUI && !WindowUI->OnCloseWindow.IsAlreadyBound(this, &UPOSettingWidget::OnCloseWindow))
    {
        WindowUI->OnCloseWindow.AddDynamic(this, &UPOSettingWidget::OnCloseWindow);
    }
    
    // 초기 슬라이더 값 (0~100 범위를 0~1로 정규화)
    if (MouseSensitivitySlider)
    {
        MouseSensitivitySlider->SetMinValue(0.f);
        MouseSensitivitySlider->SetMaxValue(1.f);
        MouseSensitivitySlider->SetValue(MouseSensitivityValue / 100.f);
        MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &UPOSettingWidget::OnMouseSensitivityChanged);
    }
    if (MouseSensitivityValueText)
    {
        UpdateValueText(MouseSensitivityValueText, MouseSensitivityValue);
    }

    if (VolumeSlider)
    {
        VolumeSlider->SetMinValue(0.f);
        VolumeSlider->SetMaxValue(1.f);
        VolumeSlider->SetValue(VolumeValue / 100.f);
        VolumeSlider->OnValueChanged.AddDynamic(this, &UPOSettingWidget::OnVolumeChanged);
    }
    if (VolumeValueText)
    {
        UpdateValueText(VolumeValueText, VolumeValue);
    }

    // 시작 시 값 적용
    ApplyMouseSensitivity(MouseSensitivityValue / 100.f);
    ApplyVolume(VolumeValue / 100.f);
}

void UPOSettingWidget::NativeDestruct()
{
    if (WindowUI && WindowUI->OnCloseWindow.IsAlreadyBound(this, &UPOSettingWidget::OnCloseWindow))
    {
        WindowUI->OnCloseWindow.RemoveDynamic(this, &UPOSettingWidget::OnCloseWindow);
    }
    
    if (MouseSensitivitySlider)
    {
        MouseSensitivitySlider->OnValueChanged.RemoveDynamic(this, &UPOSettingWidget::OnMouseSensitivityChanged);
    }
    if (VolumeSlider)
    {
        VolumeSlider->OnValueChanged.RemoveDynamic(this, &UPOSettingWidget::OnVolumeChanged);
    }

    Super::NativeDestruct();
}

void UPOSettingWidget::OnMouseSensitivityChanged(float NewValue)
{
    MouseSensitivityValue = FMath::Clamp(NewValue * 100.f, 0.f, 100.f);
    if (MouseSensitivityValueText)
    {
        UpdateValueText(MouseSensitivityValueText, MouseSensitivityValue);
    }

    ApplyMouseSensitivity(NewValue);
}

void UPOSettingWidget::OnVolumeChanged(float NewValue)
{
    VolumeValue = FMath::Clamp(NewValue * 100.f, 0.f, 100.f);
    if (VolumeValueText)
    {
        UpdateValueText(VolumeValueText, VolumeValue);
    }

    ApplyVolume(NewValue);
}

void UPOSettingWidget::OnCloseWindow()
{
    // 종료 시 한번 더 저장 보장
    if (IPOUIStackingInterface* UIStackingInterface = Cast<IPOUIStackingInterface>(GetOwningPlayer()))
    {
        UIStackingInterface->GetUIStackingComponent()->PopWidget();
    }
}

void UPOSettingWidget::ApplyMouseSensitivity(float Normalized01)
{
    // TODO: 실제 마우스 감도 적용 로직 (예: PlayerController/Character에 값 전달)
    UE_LOG(LogTemp, Log, TEXT("Apply Mouse Sensitivity: %f (0-1) -> %f (0-100)"), Normalized01, MouseSensitivityValue);
}

void UPOSettingWidget::ApplyVolume(float Normalized01)
{
    // TODO: 사운드 믹스/사운드 클래스 연동. 임시로 로그만.
    UE_LOG(LogTemp, Log, TEXT("Apply Volume: %f (0-1) -> %f (0-100)"), Normalized01, VolumeValue);
}

void UPOSettingWidget::UpdateValueText(UTextBlock* TextWidget, float Value01To100) const
{
    if (!TextWidget)
    {
        return;
    }
    const int32 DisplayInt = FMath::RoundToInt(Value01To100);
    TextWidget->SetText(FText::AsNumber(DisplayInt));
}
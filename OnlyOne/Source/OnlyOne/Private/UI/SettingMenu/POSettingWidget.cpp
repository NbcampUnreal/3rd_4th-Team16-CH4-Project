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

    // GI에서 저장된 사용자 설정을 먼저 로드하여 캐시 갱신
    LoadUserSettings();
    
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
    if (IPOUIStackingInterface* UIStackingInterface = Cast<IPOUIStackingInterface>(GetOwningPlayer()))
    {
        UIStackingInterface->GetUIStackingComponent()->PopWidget();
    }
}

void UPOSettingWidget::ApplyMouseSensitivity(float Normalized01)
{
    // 실제 마우스 감도 적용 로직 (GI에 반영)
    if (const UWorld* World = GetWorld())
    {
        if (UPOGameInstance* GI = Cast<UPOGameInstance>(World->GetGameInstance()))
        {
            GI->SetMouseSensitivity(MouseSensitivityValue); // 0~100 값 전달
        }
    }
    UE_LOG(LogTemp, Log, TEXT("Apply Mouse Sensitivity: %f (0-1) -> %f (0-100)"), Normalized01, MouseSensitivityValue);
}

void UPOSettingWidget::ApplyVolume(float Normalized01)
{
    // 실제 볼륨 적용 로직 (GI에 반영)
    if (const UWorld* World = GetWorld())
    {
        if (UPOGameInstance* GI = Cast<UPOGameInstance>(World->GetGameInstance()))
        {
            GI->SetMasterVolume(VolumeValue); // 0~100 값 전달
        }
    }
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

void UPOSettingWidget::LoadUserSettings()
{
    if (const UWorld* World = GetWorld())
    {
        if (const UPOGameInstance* GI = Cast<UPOGameInstance>(World->GetGameInstance()))
        {
            MouseSensitivityValue = FMath::Clamp(GI->GetMouseSensitivity(), 0.f, 100.f);
            VolumeValue = FMath::Clamp(GI->GetMasterVolume(), 0.f, 100.f);
        }
    }
}

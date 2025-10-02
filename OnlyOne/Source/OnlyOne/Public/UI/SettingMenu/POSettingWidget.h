// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/POBaseWidget.h"
#include "POSettingWidget.generated.h"

class UPOBaseWindow;
class USlider;
class UTextBlock;

/**
 * 설정 위젯: 마우스 감도 & 볼륨 (0~100)
 */
UCLASS()
class ONLYONE_API UPOSettingWidget : public UPOBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPOBaseWindow> WindowUI;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MouseSensitivitySlider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MouseSensitivityValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> VolumeSlider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> VolumeValueText;

	// 현재 값 캐시
	float MouseSensitivityValue = 50.f;
	float VolumeValue = 50.f; 

private:
	UFUNCTION()
	void OnMouseSensitivityChanged(float NewValue);
	UFUNCTION()
	void OnVolumeChanged(float NewValue);
	UFUNCTION()
	void OnCloseWindow();

	void ApplyMouseSensitivity(float Normalized01);
	void ApplyVolume(float Normalized01);
	void UpdateValueText(UTextBlock* TextWidget, float Value01To100) const;
};

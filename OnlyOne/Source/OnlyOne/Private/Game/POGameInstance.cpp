// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/POGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Save/POUserSettingsSave.h"
#include "Sound/SoundClass.h"

void UPOGameInstance::Init()
{
    Super::Init();
    LoadUserSettings();
    ApplyAudioVolume();
}

void UPOGameInstance::LoadUserSettings()
{
    if (USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex))
    {
        if (const UPOUserSettingsSave* Settings = Cast<UPOUserSettingsSave>(Loaded))
        {
            MouseSensitivityValue = FMath::Clamp(Settings->MouseSensitivityValue, 0.f, 100.f);
            VolumeValue = FMath::Clamp(Settings->VolumeValue, 0.f, 100.f);
            UE_LOG(LogTemp, Log, TEXT("Loaded UserSettings: Mouse=%f, Volume=%f"), MouseSensitivityValue, VolumeValue);
            return;
        }
    }

    // 저장 파일이 없거나 형식이 다르면 기본값으로 저장 파일 생성
    SaveUserSettings();
}

void UPOGameInstance::SaveUserSettings() const
{
    UPOUserSettingsSave* SaveObj = Cast<UPOUserSettingsSave>(UGameplayStatics::CreateSaveGameObject(UPOUserSettingsSave::StaticClass()));
    if (!SaveObj)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create SaveGame object for UserSettings"));
        return;
    }

    SaveObj->MouseSensitivityValue = FMath::Clamp(MouseSensitivityValue, 0.f, 100.f);
    SaveObj->VolumeValue = FMath::Clamp(VolumeValue, 0.f, 100.f);

    if (!UGameplayStatics::SaveGameToSlot(SaveObj, SaveSlotName, SaveUserIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to save UserSettings to slot: %s"), *SaveSlotName);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Saved UserSettings: Mouse=%f, Volume=%f"), SaveObj->MouseSensitivityValue, SaveObj->VolumeValue);
    }
}

void UPOGameInstance::SetMouseSensitivity(float InValue01To100)
{
    MouseSensitivityValue = FMath::Clamp(InValue01To100, 0.f, 100.f);
    SaveUserSettings();
}

void UPOGameInstance::SetMasterVolume(float InValue01To100)
{
    VolumeValue = FMath::Clamp(InValue01To100, 0.f, 100.f);
    ApplyAudioVolume();
    SaveUserSettings();
}

void UPOGameInstance::ApplyAudioVolume() const
{
    if (MasterSoundClass)
    {
        const float Normalized = FMath::Clamp(VolumeValue / 100.f, 0.f, 1.f);
        MasterSoundClass->Properties.Volume = Normalized;
        UE_LOG(LogTemp, Log, TEXT("ApplyAudioVolume -> MasterSoundClass.Volume: %f"), Normalized);
    }
}

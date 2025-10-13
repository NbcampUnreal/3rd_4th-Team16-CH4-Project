// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "POGameInstance.generated.h"

class USoundClass;

/**
 * 
 */

UCLASS()
class ONLYONE_API UPOGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	//TODO: 닉네임과 서버주소를 FJoinServerData와 통합하도록 변경해주세요.
	UFUNCTION(Category="PO|Profile")
	void SetPendingProfile(FJoinServerData& InitUserData)
	{
		JoinServerData = InitUserData;
	}
	
	UFUNCTION(Category="PO|Profile")
	const FJoinServerData& GetPendingProfile() const
	{
		return JoinServerData;
	}

	// 초기화 시 사용자 설정 불러오기
	virtual void Init() override;

	// 설정 값 접근자 (0~100)
	UFUNCTION(BlueprintCallable, Category="PO|Settings")
	void SetMouseSensitivity(float InValue01To100);
	UFUNCTION(BlueprintCallable, Category="PO|Settings")
	float GetMouseSensitivity() const { return MouseSensitivityValue; }

	UFUNCTION(BlueprintCallable, Category="PO|Settings")
	void SetMasterVolume(float InValue01To100);
	UFUNCTION(BlueprintCallable, Category="PO|Settings")
	float GetMasterVolume() const { return VolumeValue; }

	UFUNCTION(BlueprintCallable, Category="PO|Settings")
	void LoadUserSettings();
	UFUNCTION(BlueprintCallable, Category="PO|Settings")
	void SaveUserSettings() const;

private:
	UPROPERTY()
	FJoinServerData JoinServerData;

	// 로컬 저장 캐시 (0~100)
	UPROPERTY()
	float MouseSensitivityValue = 50.f;
	UPROPERTY()
	float VolumeValue = 50.f;

	// SaveGame 슬롯 정보
	FString SaveSlotName = TEXT("UserSettings");
	int32 SaveUserIndex = 0;

	// 오디오 연동: 에디터에서 마스터 사운드 클래스 할당
	UPROPERTY(EditDefaultsOnly, Category="PO|Audio")
	TObjectPtr<USoundClass> MasterSoundClass = nullptr;

	// 런타임 적용 헬퍼
	void ApplyAudioVolume() const;
};

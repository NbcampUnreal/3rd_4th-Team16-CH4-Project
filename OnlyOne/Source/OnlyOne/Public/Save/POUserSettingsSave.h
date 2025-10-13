#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "POUserSettingsSave.generated.h"

/**
 * 사용자 설정 저장용 SaveGame
 */
UCLASS()
class ONLYONE_API UPOUserSettingsSave : public USaveGame
{
	GENERATED_BODY()
public:
	// 0~100 범위 값 저장
	UPROPERTY(BlueprintReadWrite, Category="PO|Settings")
	float MouseSensitivityValue = 50.f;

	UPROPERTY(BlueprintReadWrite, Category="PO|Settings")
	float VolumeValue = 50.f;
};


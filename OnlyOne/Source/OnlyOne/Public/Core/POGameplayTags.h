// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "POGameplayTags.generated.h"

/**
 * 
 */
USTRUCT()
struct ONLYONE_API FPOGameplayTags
{
	GENERATED_BODY()

public:
	static void InitializeNativeTags();
	static bool IsInitialized() { return bIsInitialized; }

	/* Gameplay Tags */
	//TODO: 여기에 게임에서 사용할 Gameplay Tag들을 선언하세요. 또한, CPP 파일에도 추가해야 합니다.
	static FGameplayTag Test_TestTag; //NOTE: 하나 이상의 태그가 선언되면 테스트 코드는 지울 것
private:
	static bool bIsInitialized;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationInstances/POCharacterAnimInstance.h"
#include "POPlayerAnimInstance.generated.h"

/**
 * @class UPOPlayerAnimInstance
 * @brief 실제 '플레이어 캐릭터'에 사용되는 최종 애니메이션 인스턴스 클래스
 * UPOCharacterAnimInstance를 상속하며, 플레이어 고유의 애니메이션 로직 (예: 스킬 사용, 장비 상호작용)을 처리
 */
UCLASS()
class ONLYONE_API UPOPlayerAnimInstance : public UPOCharacterAnimInstance
{
	GENERATED_BODY()
	
};
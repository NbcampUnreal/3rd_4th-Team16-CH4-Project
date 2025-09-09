// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationInstances/POAnimInstanceBase.h"
#include "POCharacterAnimInstance.generated.h"

/**
 * @class UPOCharacterAnimInstance
 * @brief 플레이어, NPC 등 모든 '캐릭터' 타입의 애니메이션을 위한 중간 베이스 클래스.
 * UPOAnimInstanceBase를 상속하며, 캐릭터라면 공통적으로 가지는 이동, 점프 등의 상태 관련 로직을 처리.
 */
UCLASS()
class ONLYONE_API UPOCharacterAnimInstance : public UPOAnimInstanceBase
{
	GENERATED_BODY()
	
};
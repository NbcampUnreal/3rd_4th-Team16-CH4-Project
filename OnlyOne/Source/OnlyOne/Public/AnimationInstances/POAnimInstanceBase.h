// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "POAnimInstanceBase.generated.h"

/**
 * @class UPOAnimInstanceBase
 * @brief 프로젝트에서 사용되는 모든 커스텀 애니메이션 인스턴스의 최상위 부모 클래스
 * 이 클래스는 모든 애니메이션 인스턴스가 공유하는 공통 데이터나 함수를 정의하는 데 사용
 */
UCLASS()
class ONLYONE_API UPOAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
	
};
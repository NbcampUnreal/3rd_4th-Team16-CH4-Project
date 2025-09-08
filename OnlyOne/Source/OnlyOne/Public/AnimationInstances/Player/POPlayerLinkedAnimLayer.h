// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationInstances/POAnimInstanceBase.h"
#include "POPlayerLinkedAnimLayer.generated.h"

/**
 * @class UPOPlayerLinkedAnimLayer
 * @brief 플레이어의 특정 애니메이션 레이어를 독립적으로 처리하기 위한 클래스. (예: 상체 애니메이션)
 * UPOAnimInstanceBase를 상속하며, 주로 'Anim Layer Interface'와 함께 사용하여
 * 메인 애니메이션 블루프린트의 복잡도를 낮추고 기능을 모듈화하는 데 사용
 */
UCLASS()
class ONLYONE_API UPOPlayerLinkedAnimLayer : public UPOAnimInstanceBase
{
	GENERATED_BODY()
	
};
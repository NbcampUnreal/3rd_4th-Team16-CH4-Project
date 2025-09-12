// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PODataAsset_StartupDataBase.generated.h"

class UGameplayEffect;
class UPOGameplayAbility;
class UPOAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPODataAsset_StartupDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UPOAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);

protected:

	/** 
	 * 캐릭터가 생성되거나 ASC에 연결될 때 
	 * 자동으로 부여되어 활성화되는 어빌리티 목록
	 * 예: 이동, 공격 같은 기본 능력들
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UPOGameplayAbility>> ActivateOnGivenAbilities;

	/** 
	 * 특정 이벤트(조건)에 의해 발동되는 어빌리티 목록 
	 * 예: 피격 시 발동하는 히트 리액션, 사망 처리 등
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UPOGameplayAbility>> ReactiveAbilities;

	
	/** 
	 * 캐릭터가 생성될 때 기본적으로 적용되는 게임플레이 효과 목록
	 * 예: 초기 스탯(HP/MP), 버프/디버프, 상태 값 세팅
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;
	
	void GrantAbilities(const TArray<TSubclassOf<UPOGameplayAbility>>& InAbilitiesToGive, UPOAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
};

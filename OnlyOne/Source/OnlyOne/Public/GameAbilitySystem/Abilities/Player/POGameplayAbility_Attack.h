// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POPlayerGameplayAbility.h"
#include "POGameplayAbility_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOGameplayAbility_Attack : public UPOPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UPOGameplayAbility_Attack();

protected:
	/** 어빌리티가 활성화될 때 호출됩니다. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageEndCancelled();
	
	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData EventData);
};

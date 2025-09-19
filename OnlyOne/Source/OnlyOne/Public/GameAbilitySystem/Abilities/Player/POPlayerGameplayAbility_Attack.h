// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POPlayerGameplayAbility.h"
#include "POPlayerGameplayAbility_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPlayerGameplayAbility_Attack : public UPOPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UPOPlayerGameplayAbility_Attack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat | Animation")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat | Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageEndCancelled();
	
	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData EventData);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat | Debug")
	bool bIsDebugDraw = true;
};

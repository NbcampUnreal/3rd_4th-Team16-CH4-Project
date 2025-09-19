// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Abilities/NPC/PONPCGameplayAbility.h"
#include "PONPCGameplayAbility_Death.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPONPCGameplayAbility_Death : public UPONPCGameplayAbility
{
	GENERATED_BODY()
public:
	UPONPCGameplayAbility_Death();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	UFUNCTION()
	void OnCompletedCallback();
};

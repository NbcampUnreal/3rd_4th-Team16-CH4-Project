// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POPlayerGameplayAbility.h"
#include "POPlayerGameplayAbility_Death.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPlayerGameplayAbility_Death : public UPOPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UPOPlayerGameplayAbility_Death();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UFUNCTION()
	void OnMontageCompleted(FGameplayEventData EventData);
};

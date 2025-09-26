// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POPlayerGameplayAbility.h"
#include "POPlayerGameplayAbility_SmokeGrenade.generated.h"

class APOSmokeGrenade;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPlayerGameplayAbility_SmokeGrenade : public UPOPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UPOPlayerGameplayAbility_SmokeGrenade();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<APOSmokeGrenade> SmokeEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Smoke Grenade")
	FVector SpawnOffset = FVector(0.f, 0.f, -50.f);
};

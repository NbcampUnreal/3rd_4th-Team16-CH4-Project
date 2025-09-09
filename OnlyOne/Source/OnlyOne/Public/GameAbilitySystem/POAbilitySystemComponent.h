// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "POAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void ServerAddLooseGameplayTag(FGameplayTag TagToAdd);
};

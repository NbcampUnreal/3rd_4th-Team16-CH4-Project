// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Abilities/POGameplayAbility.h"
#include "POPlayerGameplayAbility.generated.h"

class APOPlayerCharacter;
class APOPlayerController;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOPlayerGameplayAbility : public UPOGameplayAbility
{
	GENERATED_BODY()

protected:
	TObjectPtr<APOPlayerCharacter> GetPlayerCharacterFromActorInfo();
	TObjectPtr<APOPlayerController> GetPlayerControllerFromActorInfo();

private:
	UPROPERTY()
	TObjectPtr<APOPlayerCharacter> CachedPOPlayerCharacter;

	UPROPERTY()
	TObjectPtr<APOPlayerController> CachedPOPlayerController;
};

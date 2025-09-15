// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Abilities/Player/POPlayerGameplayAbility.h"
#include "Characters/POPlayerCharacter.h"
#include "Controllers/POPlayerController.h"

TObjectPtr<APOPlayerCharacter> UPOPlayerGameplayAbility::GetPlayerCharacterFromActorInfo()
{
	if (!CachedPOPlayerCharacter)
	{
		CachedPOPlayerCharacter = Cast<APOPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	}

	return CachedPOPlayerCharacter.Get();
}

TObjectPtr<APOPlayerController> UPOPlayerGameplayAbility::GetPlayerControllerFromActorInfo()
{
	if (!CachedPOPlayerController)
	{
		CachedPOPlayerController = Cast<APOPlayerController>(CurrentActorInfo->PlayerController.Get());
	}

	return CachedPOPlayerController.Get();
}

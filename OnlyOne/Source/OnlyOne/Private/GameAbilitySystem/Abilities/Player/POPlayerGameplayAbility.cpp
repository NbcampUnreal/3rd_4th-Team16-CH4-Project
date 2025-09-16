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
		// 1. 먼저 ActorInfo에서 직접 가져오기를 시도 (주로 클라이언트에서 유효)
		CachedPOPlayerController = Cast<APOPlayerController>(CurrentActorInfo->PlayerController.Get());

		// 실패 시 AvatarActor에서 Controller를 가져옴 (주로 서버에서 유효)
		if (!CachedPOPlayerController)
		{
			if (const APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
			{
				if (AController* Controller = AvatarPawn->GetController())
				{
					CachedPOPlayerController = Cast<APOPlayerController>(Controller);
				}
			}
		}
	}

	return CachedPOPlayerController.Get();
}

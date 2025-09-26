// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Abilities/Player/POPlayerGameplayAbility_SmokeGrenade.h"
#include "POGameplayTags.h"
#include "Characters/POPlayerCharacter.h"
#include "Items/POSmokeGrenade.h"

UPOPlayerGameplayAbility_SmokeGrenade::UPOPlayerGameplayAbility_SmokeGrenade()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FGameplayTagContainer Tags;
	Tags.AddTag(POGameplayTags::Player_Ability_SmokeGrenade);
	SetAssetTags(Tags);
}

void UPOPlayerGameplayAbility_SmokeGrenade::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (ActorInfo->IsNetAuthority())
	{
		ACharacter* PlayerCharacter = Cast<APOPlayerCharacter>(GetAvatarActorFromActorInfo());
		if (PlayerCharacter && SmokeEffectClass)
		{
			const FVector CharacterLocation = PlayerCharacter->GetActorLocation();
			const FVector SpawnLocation = CharacterLocation + SpawnOffset;
			
			const FRotator SpawnRotation = PlayerCharacter->GetActorRotation();
            
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PlayerCharacter;
			SpawnParams.Instigator = PlayerCharacter;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            
			
			GetWorld()->SpawnActor<APOSmokeGrenade>(SmokeEffectClass, SpawnLocation, SpawnRotation, SpawnParams);
			
		}
	}
	// 연막탄을 생성한 후 바로 어빌리티를 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

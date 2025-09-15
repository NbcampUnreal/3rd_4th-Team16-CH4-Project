// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Abilities/Player/POPlayerGameplayAbility_Death.h"

#include "POGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/POPlayerCharacter.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"

UPOPlayerGameplayAbility_Death::UPOPlayerGameplayAbility_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTagContainer Tags;
	Tags.AddTag(POGameplayTags::Player_State_Death);
	SetAssetTags(Tags);
	
	BlockAbilitiesWithTag.AddTag(POGameplayTags::Player_Ability);
}

void UPOPlayerGameplayAbility_Death::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!GetPlayerCharacterFromActorInfo())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 사망 시 다른 모든 어빌리티를 취소
	GetPlayerCharacterFromActorInfo()->GetPOAbilitySystemComponent()->CancelAllAbilities();
	
	// 사망 몽타주를 재생합니다.
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DeathMontage);
	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		// 몽타주 재생에 실패하면 즉시 어빌리티를 종료합니다.
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UPOPlayerGameplayAbility_Death::OnMontageCompleted()
{
	{
		// 컨트롤러가 아닌 캐릭터를 가져와야 합니다.
		if (APOPlayerCharacter* CharacterToDestroy = GetPlayerCharacterFromActorInfo())
		{
			// 컨트롤러의 빙의를 해제합니다. (캐릭터가 파괴되기 전에 하는 것이 안전)
			if (AController* Controller = CharacterToDestroy->GetController())
			{
				Controller->UnPossess();
			}

			// 컨트롤러가 아닌 캐릭터를 파괴합니다.
			CharacterToDestroy->Destroy();
		}
	
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Abilities/Player/POPlayerGameplayAbility_Death.h"

#include "DebugHelper.h"
#include "POGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/POPlayerCharacter.h"
#include "Controllers/POPlayerController.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"

UPOPlayerGameplayAbility_Death::UPOPlayerGameplayAbility_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	
	FGameplayTagContainer Tags;
	Tags.AddTag(POGameplayTags::Shared_Ability_Death);
	SetAssetTags(Tags);

	CancelAbilitiesWithTag.AddTag(POGameplayTags::Player_Ability);
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

	// 캐릭터를 찾을수없을때 어빌리티 종류
	if (!GetPlayerCharacterFromActorInfo())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주가 재생되기 전에 컨트롤러 빙의 해제
	if (APOPlayerController* PlayerController = GetPlayerControllerFromActorInfo())
	{
		PlayerController->UnPossess();
	}
	
	// 사망 몽타주를 재생
	if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DeathMontage))
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		// 어떠한 이유에서든 몽타주 재생 실패시 즉시 어빌리티 종료 처리 및 캐릭터 바로 파괴
		GetPlayerCharacterFromActorInfo()->Destroy();
		// 몽타주 재생에 실패하면 즉시 어빌리티를 종료
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UPOPlayerGameplayAbility_Death::OnMontageCompleted()
{

	if (APOPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		PlayerCharacter->Destroy();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

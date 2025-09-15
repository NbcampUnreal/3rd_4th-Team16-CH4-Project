// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Abilities/Player/POGameplayAbility_Attack.h"
#include "GameAbilitySystem/POAttributeSet.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h" 
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/POCharacterBase.h"
#include "POGameplayTags.h"

UPOGameplayAbility_Attack::UPOGameplayAbility_Attack()
{
	FGameplayTagContainer Tags;
	Tags.AddTag(POGameplayTags::Player_Ability_Attack);
	SetAssetTags(Tags);
	
	BlockAbilitiesWithTag.AddTag(POGameplayTags::Player_Ability_Attack);
}

void UPOGameplayAbility_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,AttackMontage);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주가 성공적으로 완료되었을 때 실행될 함수를 바인딩
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEndCancelled);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEndCancelled);

	// POPlayerCharacter.cpp에서 오버랩 되었을때 넘어오는 태그 수신
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, POGameplayTags::Shared_Event_MeleeHit);
	EventTask->EventReceived.AddDynamic(this, &ThisClass::OnHitEventReceived);
	EventTask->ReadyForActivation();

	// 태스크를 활성화 서버에서 몽타주가 재생되고 클라이언트로 복제
	MontageTask->ReadyForActivation();
}

void UPOGameplayAbility_Attack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPOGameplayAbility_Attack::OnMontageEndCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UPOGameplayAbility_Attack::OnHitEventReceived(FGameplayEventData EventData)
{
	AActor* TargetActor = const_cast<AActor*>(EventData.Target.Get());
	
	if (GetWorld() && bIsDebugDraw)
	{
		DrawDebugSphere(GetWorld(), TargetActor->GetActorLocation(), 60.f, 24, FColor::Green, false, 1.0f);
	}
	
	if (!HasAuthority(&CurrentActivationInfo) || !DamageEffectClass || !TargetActor)
	{
		return;
	}
	
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayEffectSpecHandle DamageEffectSpec = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
        
		const APOCharacterBase* SourceCharacter = Cast<APOCharacterBase>(CurrentActorInfo->AvatarActor.Get());
		if (SourceCharacter && SourceCharacter->GetPOAttributeSet())
		{
			const float BaseDamage = SourceCharacter->GetPOAttributeSet()->GetBaseDamage();
			DamageEffectSpec.Data.Get()->SetSetByCallerMagnitude(POGameplayTags::Shared_SetByCaller_BaseDamage, BaseDamage);
		}
        
		FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetActor);
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, DamageEffectSpec, TargetDataHandle);
	}
}

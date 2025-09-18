// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Abilities/NPC/PONPCGameplayAbility_Death.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/PONPCCharacter.h"
#include "Controllers/PONPCController.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "POGameplayTags.h"
#include "DebugHelper.h"

UPONPCGameplayAbility_Death::UPONPCGameplayAbility_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags.AddTag(POGameplayTags::Shared_Ability_Death);

	FAbilityTriggerData HitCheckTrigger;
	HitCheckTrigger.TriggerTag = POGameplayTags::Shared_Status_Death;
	HitCheckTrigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(HitCheckTrigger);
}

void UPONPCGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

	APONPCCharacter* NPCCharacter = Cast<APONPCCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(NPCCharacter))
	{
		NPCCharacter->GetCharacterMovement()->DisableMovement();
		NPCCharacter->SetCapsuleCollisionNoCollision_Multicast();

		APONPCController* AIC = Cast<APONPCController>(NPCCharacter->GetController());
		if (IsValid(AIC))
		{
			AIC->UnPossess();
		}
	}

	if (DeathMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
		(
			this, 
			FName(TEXT("DeathTask")),
			DeathMontage,
			1.0f,
			NAME_None
		);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &UPONPCGameplayAbility_Death::OnCompletedCallback);
			MontageTask->ReadyForActivation();
		}
	}
}

void UPONPCGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPONPCGameplayAbility_Death::OnCompletedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

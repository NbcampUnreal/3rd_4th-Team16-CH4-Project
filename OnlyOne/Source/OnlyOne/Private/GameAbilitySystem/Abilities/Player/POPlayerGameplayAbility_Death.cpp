// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Abilities/Player/POPlayerGameplayAbility_Death.h"
#include "POGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/POPlayerCharacter.h"
#include "Controllers/POPlayerController.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UPOPlayerGameplayAbility_Death::UPOPlayerGameplayAbility_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
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

	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

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
	
	// 몽타주 재생만 시키는 Task 실행
	if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DeathMontage))
	{
		// 몽타주가 어떤 이유로든 중단되면 그냥 어빌리티를 끝냄
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->ReadyForActivation();
	}
	else
	{
		// 몽타주 재생 실패 시 즉시 종료
		GetPlayerCharacterFromActorInfo()->Destroy();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, POGameplayTags::Event_Death);
	WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageCompleted); //사망 몽타주로부터 Event_Death 태그를 수신되길 기다리며, 수신완료시 관전 모드 시작
	WaitEventTask->ReadyForActivation();
}

void UPOPlayerGameplayAbility_Death::OnMontageCompleted(FGameplayEventData EventData)
{
	APOPlayerCharacter* DeadPlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (!DeadPlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 애니메이션 링크 해제
	if (USkeletalMeshComponent* MeshComp = DeadPlayerCharacter->GetMesh())
	{
		MeshComp->SetAnimInstanceClass(nullptr);
	}

	// 움직임 비활성화
	if (UCharacterMovementComponent* MoveComp = DeadPlayerCharacter->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
		MoveComp->SetComponentTickEnabled(false);
	}

	DeadPlayerCharacter->SetActorEnableCollision(false);
	DeadPlayerCharacter->SetIsDead(true);

	// 컨트롤러의 관전 모드를 시작
	if (APOPlayerController* PlayerController = GetPlayerControllerFromActorInfo())
	{
		PlayerController->StartSpectating(DeadPlayerCharacter);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

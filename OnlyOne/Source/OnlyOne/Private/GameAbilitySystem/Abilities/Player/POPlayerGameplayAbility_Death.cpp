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

	APOPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (!PlayerCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 1. 어빌리티 시작 즉시 움직임 비활성화
	if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	// 2. Multicast와 Timer를 이용해 몽타주 재생 및 종료 처리
	if (DeathMontage)
	{
		// 서버가 모든 클라이언트에게 몽타주 재생을 명령
		PlayerCharacter->Multicast_PlayMontage(DeathMontage);

		// 로컬 플레이어는 직접 재생하고, 몽타주 길이만큼 시간이 지난 후 콜백을 받도록 타이머 설정
		FTimerDelegate MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &UPOPlayerGameplayAbility_Death::OnMontageTimerFinished);
		PlayerCharacter->PlayMontageWithCallback(DeathMontage, MontageEndedDelegate);
	}
	else
	{
		// 몽타주가 없으면 바로 종료 로직 실행
		OnMontageCompleted(FGameplayEventData());
	}
}

void UPOPlayerGameplayAbility_Death::OnMontageCompleted(FGameplayEventData EventData)
{
	APOPlayerCharacter* DeadPlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (!DeadPlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
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
		PlayerController->UnPossess();
		PlayerController->StartSpectating(DeadPlayerCharacter);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPOPlayerGameplayAbility_Death::OnMontageTimerFinished()
{
	OnMontageCompleted(FGameplayEventData());
}

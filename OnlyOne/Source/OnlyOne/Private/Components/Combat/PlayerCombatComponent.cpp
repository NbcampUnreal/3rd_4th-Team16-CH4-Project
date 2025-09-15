// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PlayerCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DebugHelper.h"
#include "POGameplayTags.h"
#include "Abilities/GameplayAbilityTypes.h"

void UPlayerCombatComponent::HandleHit(AActor* HitActor)
{
	Super::HandleHit(HitActor);

	// 공격마다 1회만 피격 판정 처리
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}
	
	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn(); // 공격자
	EventData.Target = HitActor; // 피격자

	// POGameplayAbility_Attack.cpp의 UAbilityTask_WaitGameplayEvent::WaitGameplayEvent로 태그와 payload 전송
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		POGameplayTags::Shared_Event_MeleeHit,
		EventData
	);
}

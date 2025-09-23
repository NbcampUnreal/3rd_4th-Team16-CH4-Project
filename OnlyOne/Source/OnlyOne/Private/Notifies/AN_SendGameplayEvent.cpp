// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/AN_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameAbilitySystem/Abilities/Player/POPlayerGameplayAbility_Death.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
	}
}

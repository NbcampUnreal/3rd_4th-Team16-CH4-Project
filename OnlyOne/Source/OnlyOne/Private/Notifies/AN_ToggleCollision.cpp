// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/AN_ToggleCollision.h"

#include "POFunctionLibrary.h"
#include "Components/Combat/PawnCombatComponent.h"

void UAN_ToggleCollision::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
)
{
	if (!MeshComp)
	{
		return;		
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	
	if (!OwnerActor)
	{
		return;
	}

	EPOValidType ValidType;
	if (UPawnCombatComponent* CombatComp = UPOFunctionLibrary::BP_GetPawnCombatComponentFromActor(OwnerActor, ValidType))
	{
		if (ValidType == EPOValidType::EWT_Valid)
		{
			CombatComp->ToggleCollision(true, DamageType);
		}
	}
}

void UAN_ToggleCollision::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	if (!MeshComp)
	{
		return;		
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	EPOValidType ValidType;
	if (UPawnCombatComponent* CombatComp = UPOFunctionLibrary::BP_GetPawnCombatComponentFromActor(OwnerActor, ValidType))
	{
		if (ValidType == EPOValidType::EWT_Valid)
		{
			CombatComp->ToggleCollision(false, DamageType);
		}
	}
}

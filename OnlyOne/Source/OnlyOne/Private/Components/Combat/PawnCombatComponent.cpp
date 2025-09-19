// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"
#include "Characters/POPlayerCharacter.h"
#include "Components/BoxComponent.h"

void UPawnCombatComponent::ToggleCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		if (!OwningPawn->HasAuthority())
		{
			if (OwningPawn->IsLocallyControlled())
			{
				ServerToggleCollision(bShouldEnable, ToggleDamageType);
			}
			return;
		}
	}

	// 서버에서만 실제 콜리전 변경
	HandleToggleCollision(bShouldEnable, ToggleDamageType);
}

APOPlayerCharacter* UPawnCombatComponent::GetOwnerCharacter() const
{
	if (APOPlayerCharacter* OwningCharacter = Cast<APOPlayerCharacter>(GetOwner()))
	{
		return OwningCharacter;
	}

	return nullptr;
}

void UPawnCombatComponent::HandleHit(AActor* HitActor)
{
}

void UPawnCombatComponent::HandleToggleCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	if (ToggleDamageType == EToggleDamageType::EED_LeftHand || ToggleDamageType == EToggleDamageType::EED_RightHand)
	{
		APOPlayerCharacter* OwningCharacter = GetOwnerCharacter();
		if (!OwningCharacter)
		{
			return;
		}

		UBoxComponent* CollisionBox = OwningCharacter->GetAttackHitCollisionBox();
		if (!CollisionBox)
		{
			return;
		}

		CollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		CollisionBox->SetGenerateOverlapEvents(bShouldEnable);
		
		if (!bShouldEnable)
		{
			OverlappedActors.Empty();
		}
	}
}

void UPawnCombatComponent::ServerToggleCollision_Implementation(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	HandleToggleCollision(bShouldEnable, ToggleDamageType);
}


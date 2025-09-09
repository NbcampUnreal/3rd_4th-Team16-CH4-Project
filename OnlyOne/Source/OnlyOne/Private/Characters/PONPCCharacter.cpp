// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PONPCCharacter.h"

#include "Components/Combat/NpcCombatComponent.h"

APONPCCharacter::APONPCCharacter()
{
	NpcCombatComponent = CreateDefaultSubobject<UNpcCombatComponent>(TEXT("NPC Combat Component"));
}

UPawnCombatComponent* APONPCCharacter::GetPawnCombatComponent() const
{
	return GetNpcCombatComponent();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PONPCCharacter.h"

#include "Components/Combat/NpcCombatComponent.h"

// CMC
#include "GameFramework/CharacterMovementComponent.h"

// AIController
#include "Controllers/PONPCController.h"

APONPCCharacter::APONPCCharacter()
{
	AIControllerClass = APONPCController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	NpcCombatComponent = CreateDefaultSubobject<UNpcCombatComponent>(TEXT("NPC Combat Component"));
}

UBehaviorTree* APONPCCharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}

UPawnCombatComponent* APONPCCharacter::GetPawnCombatComponent() const
{
	return GetNpcCombatComponent();
}

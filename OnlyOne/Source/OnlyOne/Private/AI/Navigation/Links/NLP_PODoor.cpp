// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Navigation/Links/NLP_PODoor.h"
#include "Map/Gimmick/PODoor.h"
#include "Characters/PONPCCharacter.h"
#include "Controllers/PONPCController.h"
#include "GameFramework/CharacterMovementComponent.h"

ANLP_PODoor::ANLP_PODoor()
{
	bSmartLinkIsRelevant = true;
	PointLinks.Empty();
}

void ANLP_PODoor::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &ANLP_PODoor::OpenDoor);
}

void ANLP_PODoor::OpenDoor(AActor* MovingActor, const FVector& DestinationPoint)
{
	if (TargetDoorActor->IsOpen())
	{
		return;
	}

	if (APONPCCharacter* NPC = Cast<APONPCCharacter>(MovingActor))
	{
		if (TargetDoorActor)
		{
			NPC->GetCharacterMovement()->DisableMovement();
			TargetDoorActor->Interact(NPC);

			FTimerHandle ResumeTimer;
			// TODO: 문 열리는 시간에 맞춰서 조정 필요
			GetWorld()->GetTimerManager().SetTimer(ResumeTimer, [this, MovingActor, DestinationPoint]() { ResumeNPC(MovingActor, DestinationPoint); }, 1.0f, false);
		}
	}
}

void ANLP_PODoor::ResumeNPC(AActor* MovingActor, const FVector& DestinationPoint)
{
	if (APONPCCharacter* NPC = Cast<APONPCCharacter>(MovingActor))
	{
		NPC->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
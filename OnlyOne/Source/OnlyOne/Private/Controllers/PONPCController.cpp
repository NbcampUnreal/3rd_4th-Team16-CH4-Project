// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PONPCController.h"

// AI
#include "Characters/PONPCCharacter.h"
#include "BehaviorTree/BehaviorTree.h"

// UI
#include "Components/UI/NpcUIComponent.h"

APONPCController::APONPCController()
{
	NpcUIComponent = CreateDefaultSubobject<UNpcUIComponent>("Npc UI Component");
}

void APONPCController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APONPCCharacter* AICharacter = Cast<APONPCCharacter>(InPawn);
	if (IsValid(AICharacter) == true)
	{
		UBehaviorTree* BT = AICharacter->GetBehaviorTree();
		if (IsValid(BT) == true)
		{
			UBlackboardComponent* BlackboardComp;
			UseBlackboard(BT->GetBlackboardAsset(), BlackboardComp);
			Blackboard = BlackboardComp;
			RunBehaviorTree(BT);
		}
	}
}

UPawnUIComponent* APONPCController::GetPawnUIComponent() const
{
	return NpcUIComponent;
}

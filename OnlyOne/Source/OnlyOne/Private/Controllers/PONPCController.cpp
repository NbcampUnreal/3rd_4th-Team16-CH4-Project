// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PONPCController.h"

#include "Characters/PONPCCharacter.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Components/UI/NpcUIComponent.h"

APONPCController::APONPCController()
{
	NpcUIComponent = CreateDefaultSubobject<UNpcUIComponent>("Npc UI Component");
}

void APONPCController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APONPCCharacter* AICharacter = Cast<APONPCCharacter>(InPawn);
	if (IsValid(AICharacter))
	{
		UBehaviorTree* BT = AICharacter->GetBehaviorTree();
		if (IsValid(BT))
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

ETeamAttitude::Type APONPCController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (GetPawn() == &Other)
	{
		return ETeamAttitude::Friendly;
	}
	
	return ETeamAttitude::Hostile;
}

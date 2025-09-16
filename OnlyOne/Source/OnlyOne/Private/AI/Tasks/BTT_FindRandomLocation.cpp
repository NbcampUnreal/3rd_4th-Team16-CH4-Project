// BTT_FindRandomLocation.cpp


#include "AI/Tasks/BTT_FindRandomLocation.h"

// AIController
#include "Controllers/PONPCController.h"

// AICharacter
#include "Characters/PONPCCharacter.h"

// Navgation
#include "NavigationSystem.h"

// Blackboard
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindRandomLocation::UBTT_FindRandomLocation()
	:SearchRadius(1500.f)
{
	NodeName = "Find Random Location In NavMesh";
}

EBTNodeResult::Type UBTT_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (APONPCController* AIController = Cast<APONPCController>(OwnerComp.GetAIOwner()))
	{
		if (APONPCCharacter* AICharacter = Cast<APONPCCharacter>(AIController->GetPawn()))
		{
			FVector OriginLocation = AICharacter->GetActorLocation();

			if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				FNavLocation NewRandomLocation;
				if (NavSys->GetRandomPointInNavigableRadius(OriginLocation, SearchRadius, NewRandomLocation))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NewRandomLocation.Location);
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}

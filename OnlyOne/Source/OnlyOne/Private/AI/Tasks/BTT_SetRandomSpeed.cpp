#include "AI/Tasks/BTT_SetRandomSpeed.h"

#include "Controllers/PONPCController.h"
#include "Characters/PONPCCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTT_SetRandomSpeed::UBTT_SetRandomSpeed()
{
	NodeName = TEXT("Set Random Speed");
}

EBTNodeResult::Type UBTT_SetRandomSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (APONPCController* AIController = Cast<APONPCController>(OwnerComp.GetAIOwner()))
	{
		if (APONPCCharacter* AICharacter = Cast<APONPCCharacter>(AIController->GetPawn()))
		{
			if (UCharacterMovementComponent* MoveComp = AICharacter->FindComponentByClass<UCharacterMovementComponent>())
			{
				float Speeds[3] = { 200.f, 500.f, 600.f };
				int32 Index = FMath::RandRange(0, 2);
				MoveComp->MaxWalkSpeed = Speeds[Index];
			}
		}
	}
	return EBTNodeResult::Succeeded;
}

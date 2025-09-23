#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetRandomSpeed.generated.h"

UCLASS()
class ONLYONE_API UBTT_SetRandomSpeed : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_SetRandomSpeed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

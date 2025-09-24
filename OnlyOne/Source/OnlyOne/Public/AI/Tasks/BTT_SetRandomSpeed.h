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

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;

	TArray<float> Speeds;
};

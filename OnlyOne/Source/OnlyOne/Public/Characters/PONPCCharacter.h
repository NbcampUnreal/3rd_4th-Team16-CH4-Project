// PONPCCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Characters/POCharacterBase.h"
#include "PONPCCharacter.generated.h"

class UNpcCombatComponent;

UCLASS()
class ONLYONE_API APONPCCharacter : public APOCharacterBase
{
	GENERATED_BODY()

public:
	APONPCCharacter();

#pragma region BehaviorTree

public:
	UBehaviorTree* GetBehaviorTree() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

#pragma endregion


#pragma region CombatComponent

public:
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	FORCEINLINE UNpcCombatComponent* GetNpcCombatComponent() const { return NpcCombatComponent; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNpcCombatComponent> NpcCombatComponent;

#pragma endregion
};

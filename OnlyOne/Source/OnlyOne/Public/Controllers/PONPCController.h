// PONPCController.cpp

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interfaces/PawnUIInterface.h"
#include "PONPCController.generated.h"

//class UNpcUIComponent;

UCLASS()
class ONLYONE_API APONPCController : public AAIController, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	APONPCController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

#pragma region UI

public:
	virtual UPawnUIComponent* GetPawnUIComponent() const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNpcUIComponent> NpcUIComponent;

#pragma endregion
};

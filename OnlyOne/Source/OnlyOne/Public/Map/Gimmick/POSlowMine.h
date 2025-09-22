#pragma once

#include "CoreMinimal.h"
#include "Map/Gimmick/POGimmickBase.h"
#include "POSlowMine.generated.h"


class UGameplayEffect;

UCLASS()
class ONLYONE_API APOSlowMine : public APOGimmickBase
{
	GENERATED_BODY()
public:
	APOSlowMine();

protected:
	virtual void OnGimmickComplete_Implementation(AActor* Target) override;

	virtual void ActivateGimmick_Implementation(AActor* Target) override;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> GE_Slow;
	
};

#pragma once

#include "CoreMinimal.h"
#include "POGameplayTags.h"
#include "Map/Gimmick/POGimmickBase.h"
#include "POSoundMine.generated.h"

class UStaticMeshComponent;

UCLASS()
class ONLYONE_API APOSoundMine : public APOGimmickBase
{
	GENERATED_BODY()

public:
	APOSoundMine();

protected:
	
	virtual void OnGimmickComplete_Implementation(AActor* Target) override;
	
	
	virtual void ActivateGimmick_Implementation(AActor* Target) override;
	
	
};

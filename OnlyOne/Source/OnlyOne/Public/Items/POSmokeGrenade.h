// OnlyOne/Public/Items/POSmokeGrenade.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POSmokeGrenade.generated.h"

class UNiagaraComponent; // UParticleSystemComponent 대신 UNiagaraComponent를 전방 선언합니다.

UCLASS()
class ONLYONE_API APOSmokeGrenade : public AActor
{
    GENERATED_BODY()

public:
    APOSmokeGrenade();

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
    TObjectPtr<UNiagaraComponent> SmokeNiagaraComponent;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    float EffectDuration = 10.0f;
};
// OnlyOne/Private/Effects/POSmokeEffect.cpp

#include "Items/POSmokeGrenade.h"
#include "NiagaraComponent.h"

APOSmokeGrenade::APOSmokeGrenade()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SmokeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeNiagaraComponent"));
    RootComponent = SmokeNiagaraComponent;
    
    SmokeNiagaraComponent->bAutoActivate = true;
    
    InitialLifeSpan = EffectDuration;
}

void APOSmokeGrenade::BeginPlay()
{
    Super::BeginPlay();
}
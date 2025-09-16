#include "Map/Gimmick/POGimmickBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"          
#include "GameplayEffectTypes.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"              


APOGimmickBase::APOGimmickBase() :
 NetCullDistance(750.f)
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true; 

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(Root);
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    BoxCollision->SetGenerateOverlapEvents(true);

    SetNetCullDistanceSquared(NetCullDistance * NetCullDistance);
}

void APOGimmickBase::BeginPlay()
{
    Super::BeginPlay();
    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &APOGimmickBase::OnBeginOverlap);
}
void APOGimmickBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetNetMode()==NM_DedicatedServer || HasAuthority())
    {
        return;
    }
    DrawDebugSphere(GetWorld(), GetActorLocation(), NetCullDistance/2.f, 16, FColor::Green, false, 0.f);
}



// Only for player controlled pawn
bool APOGimmickBase::CanActivate_Implementation(AActor* Target) const
{
    const APawn* Pawn = Cast<APawn>(Target);
    return Pawn && Pawn->IsPlayerControlled();
}

bool APOGimmickBase::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
    const FVector& SrcLocation) const
{
    bool bIsNetRelevant = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);

    if (false == bIsNetRelevant)
    {
        UE_LOG(LogTemp, Log, TEXT("%s is not relevant for(%s, %s)"), *GetName(), *RealViewer->GetName(), *ViewTarget->GetName());
    }
	
    return bIsNetRelevant;
}

void APOGimmickBase::OnBeginOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bServerOnly && !HasAuthority())
    {
        return;
    }
    if (!Other || Other == this)
    {
        return;
    }
    if (bConsumed)
    {
        return;
    }
    if (CanActivate(Other))
    {
        ActivateGimmick(Other);
    }
}

void APOGimmickBase::ActivateGimmick_Implementation(AActor* Target)
{
}

//GAS연결점 
UAbilitySystemComponent* APOGimmickBase::GetASC(AActor* Actor)
{
    if (!Actor)
    {
        return nullptr;
    }

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
    {
        return ASI->GetAbilitySystemComponent();
    }
    return Actor->FindComponentByClass<UAbilitySystemComponent>();
}


void APOGimmickBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(APOGimmickBase, bConsumed);
}

//Gimmick발동 이후 처리
void APOGimmickBase::OnGimmickComplete_Implementation(AActor* Target)
{
    if (!HasAuthority())
    {
        return;
    }

    bConsumed = true;

    BoxCollision->SetGenerateOverlapEvents(false);
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    if (bDestroyed)
    {
        SetLifeSpan(0.2f);
        Destroy();
    }
}

#include "Map/Gimmick/POGimmickBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APOGimmickBase::APOGimmickBase() 
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true; 

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(Root);
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    BoxCollision->SetGenerateOverlapEvents(true);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(Root);
    
    
}


void APOGimmickBase::BeginPlay()
{
    Super::BeginPlay();
    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &APOGimmickBase::OnBeginOverlap);

    if (HasAuthority())
    {
        const float R = NetCullDistance;                 
        SetNetCullDistanceSquared(FMath::Square(R));
    }

    // 2) 호스트/클라 공통: 렌더 컷을 같은 값으로
    if (StaticMesh)
    {
        StaticMesh->bNeverDistanceCull = false;
        StaticMesh->SetCullDistance(NetCullDistance);    
    }
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
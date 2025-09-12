#include "Map/Gimmick/POGimmickBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"          
#include "GameplayEffectTypes.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"              

APOGimmickBase::APOGimmickBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true; 

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    Box->SetupAttachment(Root);
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Box->SetGenerateOverlapEvents(true);     //  ��ħ �̺�Ʈ ����

    Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
    Audio->SetupAttachment(Root);
    Audio->bAutoActivate = false;
}

void APOGimmickBase::BeginPlay()
{
    Super::BeginPlay();
    Box->OnComponentBeginOverlap.AddDynamic(this, &APOGimmickBase::OnBeginOverlap);
}

//�÷��̾ ���� ��Ʈ���ϰ� �ִ� Pawn�� ����
bool APOGimmickBase::CanActivate_Implementation(AActor* Target) const
{
    const APawn* Pawn = Cast<APawn>(Target);
    return Pawn && Pawn->IsPlayerControlled();
}

void APOGimmickBase::ActivateGimmick_Implementation(AActor* Target)
{
    if (CueTag.IsValid())
    {
        if (UAbilitySystemComponent* ASC = GetASC(Target))
        {
            FGameplayCueParameters P;
            P.SourceObject = this;
            P.Location = GetActorLocation();
            ASC->ExecuteGameplayCue(CueTag, P);
        }
        else
        {
            Multicast_PlayLocalAudio(); // ����
        }
    }
    OnGimmickComplete(Target);
}

void APOGimmickBase::OnBeginOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bServerOnly && !HasAuthority()) return;
    if (!Other || Other == this) return;
    if (bConsumed) return;  
    if (CanActivate(Other))
    {
        ActivateGimmick(Other);
    }
}

void APOGimmickBase::Multicast_PlayLocalAudio_Implementation()
{
    if (Audio) Audio->Play();  // �������� ���� ����
}

//GAS������ 
UAbilitySystemComponent* APOGimmickBase::GetASC(AActor* Actor)
{
    if (!Actor) return nullptr;

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

//Gimmick�ߵ� ���� ó�� 
void APOGimmickBase::OnGimmickComplete_Implementation(AActor* Target)
{
    if (!HasAuthority()) return;

    bConsumed = true;

    Box->SetGenerateOverlapEvents(false);
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    if (bDestroyed)
    {
        SetLifeSpan(0.2f);
        Destroy();
    }
}

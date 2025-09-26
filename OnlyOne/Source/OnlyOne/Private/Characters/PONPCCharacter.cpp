// PONPCCharacter.cpp


#include "Characters/PONPCCharacter.h"

#include "Components/Combat/NpcCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DataAssets/Startup/PODataAsset_StartupDataBase.h"
#include "Controllers/PONPCController.h"
#include "BrainComponent.h"
#include "Net/UnrealNetwork.h"

APONPCCharacter::APONPCCharacter()
{
	AIControllerClass = APONPCController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->InitCapsuleSize(28.f, 88.f);

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 10.0f; // 서로 감지할 반경
	GetCharacterMovement()->AvoidanceWeight = 0.5f;

	NpcCombatComponent = CreateDefaultSubobject<UNpcCombatComponent>(TEXT("NPC Combat Component"));
}

void APONPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (APOStageGameState* GS = GetWorld()->GetGameState<APOStageGameState>())
		{
			GS->OnPhaseChanged.AddUObject(this, &APONPCCharacter::HandleGamePhaseChanged);
		}
	}
}

void APONPCCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!CharacterStartUpData.IsNull())
	{
		if (UPODataAsset_StartupDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			constexpr int32 AbilityApplyLevel = 1;
			LoadedData->GiveToAbilitySystemComponent(POAbilitySystemComponent, AbilityApplyLevel);
		}
	}
}

void APONPCCharacter::HandleGamePhaseChanged(EPOStagePhase NewPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	APONPCController* AIC = Cast<APONPCController>(GetController());
	if (!AIC)
	{
		return;
	}

	UBrainComponent* BrainComp = AIC->BrainComponent;
	if (!BrainComp)
	{
		return;
	}

	if (NewPhase == EPOStagePhase::Prep)
	{
		BrainComp->StopLogic(TEXT("Prep Phase - AI Disabled"));
	}
	else if (NewPhase == EPOStagePhase::Active)
	{
		BrainComp->RestartLogic();
	}
}

UBehaviorTree* APONPCCharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}

UPawnCombatComponent* APONPCCharacter::GetPawnCombatComponent() const
{
	return GetNpcCombatComponent();
}

void APONPCCharacter::SetCapsuleCollisionNoCollision_Multicast_Implementation()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

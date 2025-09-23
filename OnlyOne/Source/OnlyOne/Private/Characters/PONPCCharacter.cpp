// PONPCCharacter.cpp


#include "Characters/PONPCCharacter.h"

#include "Components/Combat/NpcCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DataAssets/Startup/PODataAsset_StartupDataBase.h"
#include "Controllers/PONPCController.h"
#include "Net/UnrealNetwork.h"

APONPCCharacter::APONPCCharacter()
{
	AIControllerClass = APONPCController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	NpcCombatComponent = CreateDefaultSubobject<UNpcCombatComponent>(TEXT("NPC Combat Component"));
}

void APONPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APOStageGameState* GS = GetWorld()->GetGameState<APOStageGameState>())
	{
		GS->OnPhaseChanged.AddUObject(this, &APONPCCharacter::HandleGamePhaseChanged);
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
	if (NewPhase == EPOStagePhase::Prep)
	{
		GetCharacterMovement()->DisableMovement();
	}
	else if (NewPhase == EPOStagePhase::Active)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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

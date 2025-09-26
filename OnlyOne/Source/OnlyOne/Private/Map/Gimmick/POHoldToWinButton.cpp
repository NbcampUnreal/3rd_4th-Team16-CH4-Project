#include "Map/Gimmick/POHoldToWinButton.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Game/POStageGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"


APOHoldToWinButton::APOHoldToWinButton()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Button = CreateDefaultSubobject<UStaticMeshComponent>("Button");
	Button->SetupAttachment(Root);

	InteractableComp = CreateDefaultSubobject<UPOInteractableComponent>("InteractableComp");
	InteractableComp->SetupAttachment(Root);
	
}


void APOHoldToWinButton::BeginPlay()
{
	Super::BeginPlay();
	
}


void APOHoldToWinButton::OnRep_Holding()
{
	if (bHolding)
	{
		StartPress();
	}
	else
	{
		EndPress();
	}
}

void APOHoldToWinButton::StartHold(APawn* InPawn)
{
	if (!HasAuthority() || bHolding || !IsValid(InPawn))
	{
		return;
	}
	bHolding = true;
	HolderPawn = InPawn;
	GetWorldTimerManager().SetTimer(
		HoldTimer,
		this,
		&APOHoldToWinButton::EndHold,
		HoldDuration,
		false);
	StartPress();
}

void APOHoldToWinButton::CancleHold()
{
	if (!HasAuthority() || !bHolding)
	{
		return;
	}
	GetWorldTimerManager().ClearTimer(HoldTimer);
	bHolding = false;
	HolderPawn = nullptr;
	
	EndPress();
}

void APOHoldToWinButton::EndHold()
{
	if (!HasAuthority())
	{
		return;
	}
	bHolding = false;
	if (APOStageGameMode* GM = GetWorld()->GetAuthGameMode<APOStageGameMode>())
	{
		//게임 끝나는 함수 
	}
	HolderPawn = nullptr;
	EndPress();
}


void APOHoldToWinButton::Interact(AActor* Interactor)
{
	APawn* Pawn = Cast<APawn>(Interactor);
	if (!Pawn)
	{
		if (AController* Controller = Cast<AController>(Interactor))
		{
			Pawn = Controller->GetPawn();
		}
	}
	if (!Pawn)
	{
		return;
	}
	if (HasAuthority())
	{
		StartHold(Pawn);
	}
	else
	{
		ServerStartHold(Pawn);
	}
}

bool APOHoldToWinButton::IsInteractable() const
{
	return !bHolding;
}

void APOHoldToWinButton::ShowInteractionUI()
{
}

void APOHoldToWinButton::HideInteractionUI()
{
}

void APOHoldToWinButton::ServerStartHold_Implementation(APawn* InPawn)
{
	StartHold(InPawn);
}

void APOHoldToWinButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APOHoldToWinButton, bHolding);
}



#include "Map/Gimmick/PODoor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APODoor::APODoor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Root);
	

}

void APODoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APODoor::ORep_IsOpen()
{
	if (bIsOpen)
	{
		PlayOpen();
	}
	else
	{
		PlayClose();
	}
}

void APODoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APODoor,bIsOpen);
}






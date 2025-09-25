#include "Map/Gimmick/PODoor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

APODoor::APODoor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Root);
	
	InteractableComp = CreateDefaultSubobject<UPOInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->SetupAttachment(Root);
	
}

void APODoor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (bIsOpen)
		{
			PlayOpen();
		}
		else
		{
			PlayClose();
		}
		FTimerHandle TestToggle;
		GetWorldTimerManager().SetTimer(
			TestToggle,
			this,
			&APODoor::ServerTestAutoToggle,
			2.0f,
			true,
			1.0f);
	}
	
}
//bIsOpen변경시에 클라이언트에 연출 동기화
void APODoor::OnRep_IsOpen()
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



void APODoor::Interact(AActor* Interactor)
{
	if (!HasAuthority())
	{
		ServerToggleDoor(Interactor);
		return;
	}

	PerformToggleDoor(Interactor);
}

bool APODoor::IsInteractable() const
{
	return true;
}

void APODoor::ShowInteractionUI()
{
}

void APODoor::HideInteractionUI()
{
}

// 서버에서 주기적으로 문 상태를 자동 토글
void APODoor::ServerTestAutoToggle()
{
	PerformToggleDoor(nullptr);
}

//현 상태를 반전시켜 토글 수행
void APODoor::PerformToggleDoor(AActor* InstigatorActor)
{
	SetDoorState(!bIsOpen, InstigatorActor);
}

// 문 상태 실제 반영 및 열림/닫힘 연출 호출
void APODoor::SetDoorState(bool bOpen, AActor* InstigatorActor)
{
	if (bIsOpen ==bOpen)
	{
		return;
	}
	bIsOpen = bOpen;

	if (bIsOpen)
	{
		PlayOpen();
	}
	else
	{
		PlayClose();
	}
}
// 서버 RPC로 전달된 토글 요청 처리
void APODoor::ServerToggleDoor_Implementation(AActor* InstigatorActor)
{
	PerformToggleDoor(InstigatorActor);
}






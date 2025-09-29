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
		/*FTimerHandle TestToggle;
		GetWorldTimerManager().SetTimer(
			TestToggle,
			this,
			&APODoor::ServerTestAutoToggle,
			2.0f,
			true,
			1.0f);*/
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
	// 이제 클라이언트는 직접 문에 RPC 호출하지 않고
	// InteractManagerComponent -> ServerTryInteract 경로에서만 서버가 호출.
	if (!HasAuthority())
	{
		return; // 서버가 아니면 무시
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
// 서버 RPC로 전달된 토글 요청 처리 (현재 경로 미사용: InteractManager에서 직접 Perform 호출 구조)
void APODoor::ServerToggleDoor_Implementation(AActor* InstigatorActor)
{
	PerformToggleDoor(InstigatorActor);
}

#include "Map/Gimmick/PODoor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"


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
	DOREPLIFETIME(APODoor, bHolding);
}



void APODoor::Interact(AActor* Interactor)
{
	if (!IsInteractable())
	{
		return;
	}
	if (!HasAuthority())
	{
		ServerStartHold(Interactor);
		return;
	}

	StartHold(Interactor);
}

bool APODoor::IsInteractable() const
{
	return !bHolding;
}

void APODoor::ShowInteractionUI()
{
}

void APODoor::HideInteractionUI()
{
}

//현 상태를 반전시켜 토글 수행
void APODoor::PerformToggleDoor(AActor* InstigatorActor)
{
	SetDoorState(!bIsOpen, InstigatorActor);
}

// 문 상태 실제 반영 및 열림/닫힘 연출 호출
void APODoor::SetDoorState(bool bOpen, AActor* InstigatorActor)
{
	if (bIsOpen == bOpen)
	{
		return;
	}

	bIsOpen = bOpen;

	if (bIsOpen)
	{
		PlayOpen();

		// 열렸을 때 자동 닫힘 예약(선택)
		if (bAutoCloseAfterOpen)
		{
			GetWorldTimerManager().ClearTimer(AutoCloseTimer);
			GetWorldTimerManager().SetTimer(
				AutoCloseTimer,
				FTimerDelegate::CreateWeakLambda(this, [this, InstigatorActor]()
				{
					// 자동으로 다시 닫기
					SetDoorState(false, InstigatorActor);
				}),
				AutoCloseDelay,
				false
			);
		}
	}
	else
	{
		PlayClose();
		// 닫히면 자동 닫힘 타이머는 정리
		GetWorldTimerManager().ClearTimer(AutoCloseTimer);
	}
}

// 서버 RPC로 전달된 토글 요청 처리 (현재 경로 미사용: InteractManager에서 직접 Perform 호출 구조)
void APODoor::ServerToggleDoor_Implementation(AActor* InstigatorActor)
{
	PerformToggleDoor(InstigatorActor);
}

void APODoor::ServerStartHold_Implementation(AActor* InstigatorActor)
{
	StartHold(InstigatorActor);
}
void APODoor::StartHold(AActor* InstigatorActor)
{
	if (!HasAuthority() || bHolding)
	{
		return;
	}

	bHolding = true;

	// HoldDuration 뒤에 토글 실행
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &APODoor::OnHoldComplete, InstigatorActor);
	GetWorldTimerManager().SetTimer(HoldTimer, Delegate, HoldDuration, false);

	// 필요하면 여기서 진행바/사운드 시작
}

void APODoor::OnHoldComplete(AActor* InstigatorActor)
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(HoldTimer);

	// 대기 완료 후 실제 토글
	PerformToggleDoor(InstigatorActor);

	// 완료되었으니 더 이상 홀딩 아님
	bHolding = false;
	
}
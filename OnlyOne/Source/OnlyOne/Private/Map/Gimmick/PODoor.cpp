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
	
	// InteractWidgetComponent 생성 및 설정
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionUIComponent"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, InteractionUIOffsetZ)); // 아이템 위쪽에 표시
	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // 3D 공간에 표시
	InteractWidgetComponent->SetDrawSize(FVector2D(1200.0f, 100.0f)); // UI 크기 설정
	InteractWidgetComponent->SetVisibility(false); // 기본적으로 숨김
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
	if (InteractWidgetComponent && InteractWidgetClass)
	{
		// 위젯 클래스가 설정되어 있으면 위젯 생성
		if (!InteractWidgetComponent->GetWidget())
		{
			InteractWidgetComponent->SetWidgetClass(InteractWidgetClass);
		}
		
		InteractWidgetComponent->SetDrawSize(FVector2D(400.0f, 100.0f)); // UI 크기 설정
		InteractWidgetComponent->SetVisibility(true);
		UE_LOG(LogTemp, Log, TEXT("Interaction UI shown for %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionUIComponent or InteractionUIWidgetClass is null for %s"), *GetName());
	}
}

void APODoor::HideInteractionUI()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(false);
		UE_LOG(LogTemp, Log, TEXT("Interaction UI hidden for %s"), *GetName());
	}
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
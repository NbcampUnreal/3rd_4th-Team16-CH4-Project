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
	
	InteractableComp = CreateDefaultSubobject<UPOInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->SetupAttachment(Root);
	
	// InteractWidgetComponent 생성 및 설정
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionUIComponent"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, InteractionUIOffsetZ)); // 아이템 위쪽에 표시
	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // 3D 공간에 표시
	InteractWidgetComponent->SetDrawSize(FVector2D(200.0f, 100.0f)); // UI 크기 설정
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
	if (InteractWidgetComponent && InteractWidgetClass)
	{
		// 위젯 클래스가 설정되어 있으면 위젯 생성
		if (!InteractWidgetComponent->GetWidget())
		{
			InteractWidgetComponent->SetWidgetClass(InteractWidgetClass);
		}
		
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

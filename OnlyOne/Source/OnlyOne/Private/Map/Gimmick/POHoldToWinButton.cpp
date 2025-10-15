#include "Map/Gimmick/POHoldToWinButton.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Game/POStageGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "POGameplayTags.h"
#include "GameFramework/Controller.h"
#include "game/POLobbyPlayerState.h"


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

	
	// InteractWidgetComponent 생성 및 설정
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionUIComponent"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, InteractionUIOffsetZ)); // 아이템 위쪽에 표시
	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // 3D 공간에 표시
	InteractWidgetComponent->SetDrawSize(FVector2D(600.0f, 100.0f)); // UI 크기 설정
	InteractWidgetComponent->SetVisibility(false); // 기본적으로 숨김
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
	AnchorLocation = InPawn->GetActorLocation();
	UAbilitySystemComponent* ASC = nullptr;
	if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(HolderPawn))
	{
		ASC = ASI->GetAbilitySystemComponent();
	}
	if (!ASC)
	{
		ASC = HolderPawn->FindComponentByClass<UAbilitySystemComponent>();
	}
	HolderAsc = ASC;
	// Dead 태그(Shared_Ability_Death) 구독
	if (ASC)
	{
		DeadAbilityTagHandle =
			ASC->RegisterGameplayTagEvent(POGameplayTags::Shared_Ability_Death, EGameplayTagEventType::NewOrRemoved)
			   .AddUObject(this, &ThisClass::OnDeathTagChanged);
	}

	GetWorldTimerManager().SetTimer(
		HoldTimer,
		this,
		&APOHoldToWinButton::EndHold,
		HoldDuration,
		false);
	GetWorldTimerManager().SetTimer(
		MoveCheckTimer,
		this,
		&APOHoldToWinButton::TickHoldCheck,
		MoveCheckInterval,
		true);
	StartPress();
}

void APOHoldToWinButton::CancelHold()
{
	if (!HasAuthority() || !bHolding)
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(HoldTimer);
	GetWorldTimerManager().ClearTimer(MoveCheckTimer);

	if (HolderAsc.IsValid() && DeadAbilityTagHandle.IsValid())
	{
		HolderAsc->UnregisterGameplayTagEvent(
			DeadAbilityTagHandle,
			POGameplayTags::Shared_Ability_Death,
			EGameplayTagEventType::NewOrRemoved
		);
		DeadAbilityTagHandle.Reset();
	}
	bHolding = false;
	HolderPawn = nullptr;
	HolderAsc= nullptr;
	AnchorLocation = FVector::ZeroVector;
	EndPress();
}

void APOHoldToWinButton::EndHold()
{
	if (!HasAuthority())
	{
		return;
	}
	GetWorldTimerManager().ClearTimer(MoveCheckTimer);
	if (HolderAsc.IsValid() && DeadAbilityTagHandle.IsValid())
	{
		HolderAsc->UnregisterGameplayTagEvent(
			DeadAbilityTagHandle,
			POGameplayTags::Shared_Ability_Death,
			EGameplayTagEventType::NewOrRemoved
		);
		DeadAbilityTagHandle.Reset();
	}

	bHolding = false;
	if (APOStageGameMode* GM = GetWorld()->GetAuthGameMode<APOStageGameMode>())
	{
		// 승자 정하고 끝내려면
		APOLobbyPlayerState* Winner = nullptr;
		if (HolderPawn)
		{
			if (AController* C = HolderPawn->GetController())
			{
				Winner = Cast<APOLobbyPlayerState>(C->PlayerState);
			}
		}
		GM->EndGameForGimmick(Winner);  
	}
	HolderPawn = nullptr;
	HolderAsc = nullptr;
	AnchorLocation = FVector::ZeroVector;
	EndPress();
}
void APOHoldToWinButton::TickHoldCheck()
{
	if (!HasAuthority() || !bHolding)
	{
		return;
	}

	// Pawn 유효성/사망 처리
	if (!IsValid(HolderPawn))
	{
		CancelHold();
		return;
	}

	AController* C = HolderPawn->GetController();
	if (!IsValid(C) || !IsValid(C->PlayerState))
	{
		CancelHold();
		return;
	}
	

	// 이동 감지: 기준점 반경 이탈 시 취소
	const float DistSq = FVector::DistSquared(HolderPawn->GetActorLocation(), AnchorLocation);
	if (DistSq > FMath::Square(MoveCancelRadius))
	{
		CancelHold();
		return;
	}
}
void APOHoldToWinButton::OnDeathTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!HasAuthority() || !bHolding) return;

	// 죽음 태그가 추가되면(NewCount > 0) 취소
	if (NewCount > 0)
	{
		CancelHold();
	}
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

void APOHoldToWinButton::HideInteractionUI()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(false);
		UE_LOG(LogTemp, Log, TEXT("Interaction UI hidden for %s"), *GetName());
	}
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


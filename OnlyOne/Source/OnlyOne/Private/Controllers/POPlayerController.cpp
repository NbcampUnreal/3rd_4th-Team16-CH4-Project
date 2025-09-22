// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/POPlayerController.h"
#include "Characters/POCharacterBase.h"
#include "Characters/POPlayerCharacter.h"
#include "Components/Input/POInputComponent.h"
#include "Components/UI/PlayerUIComponent.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "POGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

class UEnhancedInputLocalPlayerSubsystem;

APOPlayerController::APOPlayerController()
{
	PlayerUIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("Player UI Component"));
}

UPawnUIComponent* APOPlayerController::GetPawnUIComponent() const
{
	return PlayerUIComponent;
}

FGenericTeamId APOPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void APOPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

void APOPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APOPlayerController, TeamID);
}

void APOPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (InputConfigDataAsset)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
		}
	}

	if (HasAuthority())
	{
		if (APOCharacterBase* POCharacterBase = Cast<APOCharacterBase>(InPawn))
		{
			POCharacterBase->SetGenericTeamId(TeamID);
		}
	}
}

ETeamAttitude::Type APOPlayerController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (GetPawn() == &Other)
	{
		return ETeamAttitude::Friendly;
	}
	
	return ETeamAttitude::Hostile;
}

void APOPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UPOInputComponent* POInputComponent = Cast<UPOInputComponent>(InputComponent))
	{
		POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Spectator_Next, ETriggerEvent::Triggered, this, &ThisClass::SpectatorNextTarget);
		POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Spectator_Previous, ETriggerEvent::Triggered, this, &ThisClass::SpectatorPreviousTarget);
	}
}

void APOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	
	if (InputConfigDataAsset)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
		}
	}
}

void APOPlayerController::StartSpectating(const APawn* DeadCharacter)
{
	if (!DeadCharacter)
	{
		return;		
	}

	// Pawn은 Destroy될 수 있으므로 PlayerState만 저장
	DiedPlayerState = DeadCharacter->GetPlayerState();

	PlayerState->SetIsSpectator(true);
	ChangeState(NAME_Spectating);

	// 관전 대상 초기화 (죽을 때 한 번만)
	BuildSpectatorTargets();

	// 시작 시 첫 번째 대상 보기
	if (SpectatorTargets.Num() > 0)
	{
		CurrentSpectatorIndex = 0;
		SetViewTargetWithBlend(SpectatorTargets[CurrentSpectatorIndex], 0.5f);
	}
}

void APOPlayerController::BuildSpectatorTargets()
{
	SpectatorTargets.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APOPlayerCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		APOPlayerCharacter* TargetPlayerCharacter = Cast<APOPlayerCharacter>(Actor);
		if (!TargetPlayerCharacter) continue;

		// 캐릭터가 살아있는지, 그리고 방금 죽은 자기 자신이 아닌지 확인
		const bool bIsAlive = TargetPlayerCharacter->GetPOAbilitySystemComponent() && !TargetPlayerCharacter->GetPOAbilitySystemComponent()->HasMatchingGameplayTag(POGameplayTags::Shared_Status_Death);
		const bool bIsNotSelf = TargetPlayerCharacter->GetPlayerState() != DiedPlayerState;

		if (bIsAlive && bIsNotSelf)
		{
			SpectatorTargets.Add(TargetPlayerCharacter);
		}
	}
}

void APOPlayerController::CycleSpectator(int32 Direction)
{
	if (!(PlayerState && PlayerState->IsSpectator()))
	{
		return;
	}
	
	BuildSpectatorTargets();

	if (SpectatorTargets.Num() == 0)
	{
		SetViewTargetWithBlend(this, 0.5f);
		return;
	}

	const int32 TargetCount = SpectatorTargets.Num();
	CurrentSpectatorIndex = (CurrentSpectatorIndex + Direction + TargetCount) % TargetCount;
    
	SetViewTargetWithBlend(SpectatorTargets[CurrentSpectatorIndex], 0.5f);
}

void APOPlayerController::SpectatorNextTarget()
{
	CycleSpectator(1);
}

void APOPlayerController::SpectatorPreviousTarget()
{
	CycleSpectator(-1);
}

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
#include "UI/PlayerStateList/POPlayerStateListWidget.h"

class UEnhancedInputLocalPlayerSubsystem;

APOPlayerController::APOPlayerController()
{
	PlayerUIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("Player UI Component"));
	OnSetPlayerStateEntry.AddUObject(this, &ThisClass::OnPlayerStateUpdated);
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
		POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Spectator_Next, ETriggerEvent::Completed, this, &ThisClass::SpectatorNextTarget);
		POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Spectator_Previous, ETriggerEvent::Completed, this, &ThisClass::SpectatorPreviousTarget);
	}

	// 테스트용: Tab 키 Press/Release로 플레이어 상태 리스트 표시/숨김
	// Enhanced Input DataAsset 설정 없이 직접 키 바인딩 (임시)
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::Tab, EInputEvent::IE_Pressed, this, &ThisClass::ShowListWidget);
		InputComponent->BindKey(EKeys::Tab, EInputEvent::IE_Released, this, &ThisClass::HideListWidget);
	}
}

void APOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
	ShowHUDWidget();

	
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
		if (!TargetPlayerCharacter)
		{
			continue;
		} 

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

void APOPlayerController::EnsureListWidgetCreated()
{
	if (PlayerStateListWidget == nullptr)
	{
		if (!PlayerStateListWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerStateListWidgetClass is not set on %s"), *GetName());
			return;
		}

		PlayerStateListWidget = CreateWidget<UPOPlayerStateListWidget>(this, PlayerStateListWidgetClass);
		//OnSetPlayerStateEntry.AddUObject(PlayerStateListWidget, &UPOPlayerStateListWidget::SetPlayerStateEntry);
		if (!PlayerStateListWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create PlayerStateListWidget"));
		}
	}
}

void APOPlayerController::ShowListWidget()
{
	EnsureListWidgetCreated();
	if (PlayerStateListWidget)
	{
		if (!PlayerStateListWidget->IsInViewport())
		{
			PlayerStateListWidget->AddToViewport();
		}
		if (!PlayerStateQueue.IsEmpty())
		{
			FPOPlayerStateEntry Entry;
			while (PlayerStateQueue.Dequeue(Entry))
			{
				PlayerStateListWidget->SetPlayerStateEntry(Entry.Nickname, Entry.bIsAlive, Entry.KillCount);
			}
		}
		PlayerStateListWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void APOPlayerController::HideListWidget()
{
	if (PlayerStateListWidget)
	{
		PlayerStateListWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void APOPlayerController::ShowHUDWidget()
{
	//TODO: UI Stacking 수정 후 리팩토링 필요
	if (HUDWidgetClass && !HUDWidgetInstance)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport(/*ZOrder=*/10);
		}
	}
}

void APOPlayerController::HideHUDWidget()
{
}

void APOPlayerController::OnPlayerStateUpdated(const FString& Nickname, bool bIsAlive, int32 KillCount)
{
	FPOPlayerStateEntry Entry;
	Entry.Nickname = Nickname;
	Entry.bIsAlive = bIsAlive;
	Entry.KillCount = KillCount;

	if (PlayerStateListWidget && PlayerStateListWidget->IsInViewport() && PlayerStateListWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		PlayerStateListWidget->SetPlayerStateEntry(Nickname, bIsAlive, KillCount);
	}
	else
	{
		PlayerStateQueue.Enqueue(Entry);
	}
}

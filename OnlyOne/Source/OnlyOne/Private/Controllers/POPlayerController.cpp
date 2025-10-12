// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/POPlayerController.h"

#include "DebugHelper.h"
#include "Characters/POCharacterBase.h"
#include "Characters/POPlayerCharacter.h"
#include "Components/Input/POInputComponent.h"
#include "Components/UI/PlayerUIComponent.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "POGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Controllers/Components/POUIStackingComponent.h"
#include "Game/POStageGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OnlyOne/OnlyOne.h"
#include "UI/Common/POExitGameWidget.h"
#include "UI/Common/POReturnLobbyWidget.h"
#include "UI/InGameMenu/POInGameMenuWidget.h"
#include "UI/PlayerStateList/POPlayerStateListWidget.h"
#include "UI/SettingMenu/POSettingWidget.h"
#include "UI/Timer/POPrevTimerWidget.h"

class UEnhancedInputLocalPlayerSubsystem;

APOPlayerController::APOPlayerController()
{
	PlayerUIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("Player UI Component"));
	
	UIStackingComponent = CreateDefaultSubobject<UPOUIStackingComponent>(TEXT("UI Stacking Component"));
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

	// 새로 소유(리스폰 등)되면 스펙테이터 도움말은 숨긴다
	HideSpectatorHelpWidget();
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
		POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_ListWidget, ETriggerEvent::Triggered, this, &ThisClass::ToggleListWidget);
		POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_EscapeMenu, ETriggerEvent::Triggered, this, &ThisClass::OnEscapeMenu);
	}
}

void APOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
	// 로컬 컨트롤러에서만 HUD 위젯을 생성
	if (IsLocalController())
	{
		ShowHUDWidget();
	}

	if (APOStageGameState* StageGS = GetWorld() ? GetWorld()->GetGameState<APOStageGameState>() : nullptr)
	{
		StageGS->OnPhaseChanged.AddUObject(this, &ThisClass::OnChangeGamePhase);
	}
	
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
	// Pawn은 Destroy될 수 있으므로 PlayerState만 저장 & nullptr 허용
	DiedPlayerState = DeadCharacter ? DeadCharacter->GetPlayerState() : nullptr;

	PlayerState->SetIsSpectator(true);
	ChangeState(NAME_Spectating);

	// 관전 정보 UI 표시
	ShowSpectatorHelpWidget();

	// 관전 대상 초기화 (죽을 때 한 번만)
	BuildSpectatorTargets();

	// 시작 시 첫 번째 대상 보기
	if (SpectatorTargets.Num() > 0)
	{
		CurrentSpectatorIndex = 0;
		SetViewTargetWithBlend(SpectatorTargets[CurrentSpectatorIndex], 0.5f);
	}
	else
	{
		SetViewTargetWithBlend(this, 0.5f);
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
		
		APlayerState* TargetPlayerState = TargetPlayerCharacter->GetPlayerState();
		
		const bool bIsNotSelf = !DiedPlayerState || TargetPlayerState != DiedPlayerState;
		const bool bIsAliveAndNotSpectating = TargetPlayerState && !TargetPlayerState->IsSpectator();

		if (bIsAliveAndNotSpectating && bIsNotSelf)
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

void APOPlayerController::OnChangeGamePhase(EPOStagePhase NewPhase)
{
	if (NewPhase == EPOStagePhase::Prep)
	{
		ShowPrevTimerWidget();
	}
	else
	{
		HidePrevTimerWidget();
	}
}

void APOPlayerController::EnsureListWidgetCreated()
{
	if (!IsLocalController())
	{
		return;
	}
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
	if (!IsLocalController())
	{
		return;
	}
	if (HUDWidgetClass && !HUDWidgetInstance)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			UIStackingComponent->SetDefaultWidget(HUDWidgetInstance, false);
		}
	}
}

void APOPlayerController::HideHUDWidget()
{
}

void APOPlayerController::ToggleListWidget()
{
	if (bIsListVisible)
	{
		HideListWidget();
	}
	else
	{
		ShowListWidget();
	}

	bIsListVisible = !bIsListVisible;
}

void APOPlayerController::ShowInGameMenuWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	if (InGameMenuWidgetClass)
	{
		if (!InGameMenuWidgetInstance)
		{
			InGameMenuWidgetInstance = CreateWidget<UPOInGameMenuWidget>(this, InGameMenuWidgetClass);
			if (InGameMenuWidgetInstance)
			{
				UIStackingComponent->PushWidget(InGameMenuWidgetInstance);
			}
		}
		else
		{
			UIStackingComponent->PushWidget(InGameMenuWidgetInstance);
		}
	}
}

void APOPlayerController::ShowSettingWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	if (SettingWidgetClass)
	{
		if (!SettingWidgetInstance)
		{
			SettingWidgetInstance = CreateWidget<UPOSettingWidget>(this, SettingWidgetClass);
			if (SettingWidgetInstance)
			{
				UIStackingComponent->PushWidget(SettingWidgetInstance);
			}
		}
		else
		{
			UIStackingComponent->PushWidget(SettingWidgetInstance);
		}
	}
}

void APOPlayerController::ShowRetunToLobbyWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	if (ReturnToLobbyWidgetClass)
	{
		if (!ReturnToLobbyWidgetInstance)
		{
			ReturnToLobbyWidgetInstance = CreateWidget<UPOReturnLobbyWidget>(this, ReturnToLobbyWidgetClass);
			if (ReturnToLobbyWidgetInstance)
			{
				UIStackingComponent->PushWidget(ReturnToLobbyWidgetInstance);
			}
		}
		else
		{
			UIStackingComponent->PushWidget(ReturnToLobbyWidgetInstance);
		}
	}
}

void APOPlayerController::ShowPrevTimerWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	if (PrevTimerWidgetClass)
	{
		if (!PrevTimerWidgetInstance)
		{
			PrevTimerWidgetInstance = CreateWidget<UPOPrevTimerWidget>(this, PrevTimerWidgetClass);
			if (PrevTimerWidgetInstance)
			{
				PrevTimerWidgetInstance->AddToViewport();
			}
		}
		else
		{
			PrevTimerWidgetInstance->AddToViewport();
		}
	}
}

void APOPlayerController::HidePrevTimerWidget()
{
	if (PrevTimerWidgetInstance)
	{
		PrevTimerWidgetInstance->RemoveFromParent();
	}
}

void APOPlayerController::ShowExitGameWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	if (ExitGameWidgetClass)
	{
		if (!ExitGameWidget)
		{
			ExitGameWidget = CreateWidget<UPOExitGameWidget>(this, ExitGameWidgetClass);
		}
		if (ExitGameWidget)
		{
			UIStackingComponent->PushWidget(ExitGameWidget);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

// 스펙테이터 도움말 위젯 표시/숨김
void APOPlayerController::ShowSpectatorHelpWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	if (SpectatorHelpWidgetClass)
	{
		if (!SpectatorHelpWidget)
		{
			SpectatorHelpWidget = CreateWidget<UUserWidget>(this, SpectatorHelpWidgetClass);
		}
		if (SpectatorHelpWidget && !SpectatorHelpWidget->IsInViewport())
		{
			SpectatorHelpWidget->AddToViewport();
		}
	}
}

void APOPlayerController::HideSpectatorHelpWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	if (SpectatorHelpWidget)
	{
		SpectatorHelpWidget->RemoveFromParent();
	}
}

void APOPlayerController::OnEscapeMenu()
{
	if (UIStackingComponent->GetStackSize() <= 1)
	{
		ShowInGameMenuWidget();
	}
	else
	{
		UIStackingComponent->PopWidget();
	}
}

UPOUIStackingComponent* APOPlayerController::GetUIStackingComponent() const
{
	return UIStackingComponent;
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
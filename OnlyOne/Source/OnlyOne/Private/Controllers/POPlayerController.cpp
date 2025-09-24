// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/POPlayerController.h"

#include "DebugHelper.h"
#include "Characters/POCharacterBase.h"
#include "Components/UI/PlayerUIComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/PlayerStateList/POPlayerStateListWidget.h"

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

void APOPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
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

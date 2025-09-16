// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/POCharacterBase.h"
#include "Controllers/POPlayerController.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"
#include "GameAbilitySystem/POAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

APOCharacterBase::APOCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	POAbilitySystemComponent = CreateDefaultSubobject<UPOAbilitySystemComponent>("OnlyOne Ability System Component");
	POAttributeSet = CreateDefaultSubobject<UPOAttributeSet>("OnlyOne Attribute Set");
}

void APOCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* APOCharacterBase::GetAbilitySystemComponent() const
{
	return GetPOAbilitySystemComponent();
}

void APOCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APOCharacterBase, TeamID);
}

void APOCharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId APOCharacterBase::GetGenericTeamId() const
{
	return TeamID;
}

UPawnCombatComponent* APOCharacterBase::GetPawnCombatComponent() const
{
	return nullptr;
}

void APOCharacterBase::InitializeAbilitySystemFromDataAsset()
{
	POAbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("start up 데이터를 character에 할당하는 것을 잊었습니다 %s!"), *GetName());
}

void APOCharacterBase::SetIsDead(bool bNewIsDead)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (bIsDead)
	{
		return;
	}

	// 사망 상태를 true로 설정 (이것이 클라이언트로 복제)
	bIsDead = bNewIsDead;
	OnRep_IsDead();
}

void APOCharacterBase::OnRep_IsDead()
{
	// 이 캐릭터를 관전하고 있을 수 있는 모든 플레이어 컨트롤러를 확인하여 관전 대상을 갱신하도록 요
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APOPlayerController* PlayerController = Cast<APOPlayerController>(*It))
			{
				// 만약 어떤 플레이어가 이 죽은 캐릭터를 보고 있었다면, 다음 관전 대상을 자동 탐색
				if (PlayerController->GetViewTarget() == this)
				{
					PlayerController->SpectatorNextTarget();
				}
			}
		}
	}
}

void APOCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 해당 경우 AI controller
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
	
}

void APOCharacterBase::ServerSideInit()
{
	if (POAbilitySystemComponent)
	{
		POAbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAbilitySystemFromDataAsset();		
	}
}

void APOCharacterBase::ClientSideInit()
{
	if (POAbilitySystemComponent)
	{
		POAbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAbilitySystemFromDataAsset();
	}
}

void APOCharacterBase::OnRep_TeamID()
{
}

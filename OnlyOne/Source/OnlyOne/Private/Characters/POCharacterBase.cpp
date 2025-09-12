// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/POCharacterBase.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"
#include "GameAbilitySystem/POAttributeSet.h"
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

UPawnCombatComponent* APOCharacterBase::GetPawnCombatComponent() const
{
	return nullptr;
}

void APOCharacterBase::InitializeAbilitySystemFromDataAsset()
{
	POAbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("start up 데이터를 character에 할당하는 것을 잊었습니다 %s!"), *GetName());
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
	// TODO: 서버사이드에 추가 로직 추가 예정
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

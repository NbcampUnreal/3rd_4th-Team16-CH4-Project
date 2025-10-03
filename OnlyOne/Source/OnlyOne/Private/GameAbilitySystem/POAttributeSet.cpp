	// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/POAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "POGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "game/POStageGameMode.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"

UPOAttributeSet::UPOAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitBaseDamage(1.f);
	InitDamageTaken(0.f);
}

void UPOAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 어떤 프로퍼티를 네트워크로 동기화할지 등록하는 매크로함수
	DOREPLIFETIME_CONDITION_NOTIFY(UPOAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UPOAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UPOAttributeSet, BaseDamage, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UPOAttributeSet, DamageTaken, COND_None, REPNOTIFY_Always)
}

void UPOAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UPOAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float LocalDamageDone = GetDamageTaken();

		if (LocalDamageDone > 0.f)
		{
			const float OldHealth = GetCurrentHealth();
			// 최종적으로 체력을 감소
			SetCurrentHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.f, GetMaxHealth()));
		}
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth()));
	}

	if (GetCurrentHealth() <= 0.f)
	{
		if (UPOAbilitySystemComponent* AbilitySystemComponent = Cast<UPOAbilitySystemComponent>(GetOwningAbilitySystemComponent()))
		{
			AActor* OwnerActor = AbilitySystemComponent->GetAvatarActor();
			if (OwnerActor)
			{
				// 기존 코드 흐름을 유지하면서 GameMode에 사망을 알리는 기능만 추가
				if (OwnerActor->HasAuthority())
				{
					APOStageGameMode* StageGameMode = nullptr;
					AController* VictimController = nullptr;
					AController* KillerController = nullptr;

					if (const UWorld* World = OwnerActor->GetWorld())
					{
						StageGameMode = Cast<APOStageGameMode>(World->GetAuthGameMode());

						// Victim Controller
						if (const APawn* OwnerPawn = Cast<APawn>(OwnerActor))
						{
							VictimController = OwnerPawn->GetController();
						}

						// Killer Controller
						if (AActor* Instigator = Data.EffectSpec.GetEffectContext().GetInstigator())
						{
							if (const APawn* InstigatorPawn = Cast<APawn>(Instigator))
							{
								KillerController = InstigatorPawn->GetController();
							}
						}
					}
					
					StageGameMode->NotifyCharacterDied(VictimController, KillerController);
				}
				
				// 사망 상태 태그를 부여
				AbilitySystemComponent->AddLooseGameplayTag(POGameplayTags::Shared_Ability_Death);

				// 사망 이벤트 발생
				FGameplayEventData Payload;
				Payload.Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
				Payload.Target = OwnerActor;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, POGameplayTags::Shared_Status_Death, Payload);
			}
		}
	}
}

void UPOAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPOAttributeSet, CurrentHealth, OldValue);
}

void UPOAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPOAttributeSet, MaxHealth, OldValue); 
}

void UPOAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPOAttributeSet, BaseDamage, OldValue);
}

void UPOAttributeSet::OnRep_DamageTaken(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPOAttributeSet, DamageTaken, OldValue);
}

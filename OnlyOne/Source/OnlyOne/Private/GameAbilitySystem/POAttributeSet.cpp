// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/POAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

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
			// 최종적으로 체력을 감소시킵니다.
			SetCurrentHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.f, GetMaxHealth()));
		}
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth()));
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

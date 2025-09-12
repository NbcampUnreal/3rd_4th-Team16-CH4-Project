// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/Startup/PODataAsset_StartupDataBase.h"
#include "GameAbilitySystem/Abilities/POGameplayAbility.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"

void UPODataAsset_StartupDataBase::GiveToAbilitySystemComponent(
	UPOAbilitySystemComponent* InASCToGive,
	int32 ApplyLevel
)
{
	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectsClass : StartUpGameplayEffects)
		{
			if (!EffectsClass)
			{
				continue;
			}
			const UGameplayEffect* EffectClassDefaultObject = EffectsClass->GetDefaultObject<UGameplayEffect>();
			InASCToGive->ApplyGameplayEffectToSelf(EffectClassDefaultObject, ApplyLevel, InASCToGive->MakeEffectContext());
			
		}
	}
}

void UPODataAsset_StartupDataBase::GrantAbilities(
	const TArray<TSubclassOf<UPOGameplayAbility>>& InAbilitiesToGive,
	UPOAbilitySystemComponent* InASCToGive, int32 ApplyLevel
)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UPOGameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!Ability)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		
		InASCToGive->GiveAbility(AbilitySpec);
	}
}

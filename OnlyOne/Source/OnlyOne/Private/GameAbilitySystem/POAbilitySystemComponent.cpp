// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/POAbilitySystemComponent.h"

void UPOAbilitySystemComponent::ServerAddLooseGameplayTag_Implementation(FGameplayTag TagToAdd)
{
	if (!HasMatchingGameplayTag(TagToAdd))
	{
		AddLooseGameplayTag(TagToAdd);
	}
}

void UPOAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}
		
		TryActivateAbility(AbilitySpec.Handle);
	}
}


void UPOAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

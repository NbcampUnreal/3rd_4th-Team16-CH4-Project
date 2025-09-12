// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/Startup/PODataAsset_PlayerStartupData.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"
#include "GameAbilitySystem/Abilities/Player/POPlayerGameplayAbility.h"
#include "POTypes/POStructTypes.h"

void UPODataAsset_PlayerStartupData::GiveToAbilitySystemComponent(UPOAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	for (const FPOPlayerAbilitySet& AbilitySet : PlayerStartUpAbilitySets)
	{
		if(!AbilitySet.IsValid())
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		
		InASCToGive->GiveAbility(AbilitySpec);
	}
}

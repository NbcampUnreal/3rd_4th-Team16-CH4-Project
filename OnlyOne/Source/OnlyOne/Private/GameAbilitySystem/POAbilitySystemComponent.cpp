// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/POAbilitySystemComponent.h"

void UPOAbilitySystemComponent::ServerAddLooseGameplayTag_Implementation(FGameplayTag TagToAdd)
{
	if (!HasMatchingGameplayTag(TagToAdd))
	{
		AddLooseGameplayTag(TagToAdd);
	}
}

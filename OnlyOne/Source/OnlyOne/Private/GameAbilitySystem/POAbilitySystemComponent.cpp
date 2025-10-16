// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/POAbilitySystemComponent.h"

#include "OnlyOne/OnlyOne.h"

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

	int32 FoundMatchingAbilities = 0;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		// <<< 디버깅 로그 추가 시작 >>>
		if (AbilitySpec.Ability)
		{
			// 어떤 어빌리티가 이 입력 태그에 반응하는지 로그를 출력합니다.
			UE_LOG(POLog, Warning, TEXT("입력 태그 [%s]에 일치하는 어빌리티를 찾았습니다: %s"), *InputTag.ToString(), *AbilitySpec.Ability->GetName());
			FoundMatchingAbilities++;
		}
		// <<< 디버깅 로그 추가 끝 >>>

		TryActivateAbility(AbilitySpec.Handle);
	}

	if (FoundMatchingAbilities > 1)
	{
		UE_LOG(POLog, Error, TEXT("치명적 오류: 동일한 입력 태그 '%s'에 대해 %d개의 어빌리티가 발견되었습니다. 중복 활성화의 원인이 됩니다!"), *InputTag.ToString(), FoundMatchingAbilities);
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

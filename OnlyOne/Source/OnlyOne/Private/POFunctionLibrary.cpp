// Fill out your copyright notice in the Description page of Project Settings.


#include "POFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"


/**
 * 특정 액터로부터 UPOAbilitySystemComponent를 가져오는 함수입니다.
 *
 * 주어진 액터에서 Ability System Component를 검색하며, 검색된 컴포넌트를
 * UPOAbilitySystemComponent로 안전하게 캐스팅하여 반환합니다.
 *
 * @param InActor   UPOAbilitySystemComponent를 가져올 대상 액터.
 *
 * @return          주어진 액터에서 UPOAbilitySystemComponent를 반환.
 *                  유효하지 않거나 캐스팅에 실패한 경우, CastChecked로 인해 실행 중 오류가 발생합니다.
 */
UPOAbilitySystemComponent* UPOFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor)

	return CastChecked<UPOAbilitySystemComponent>(UAbilitySystemBlueprintLibrary:: GetAbilitySystemComponent(InActor));
}


/**
 * 액터에 지정된 GameplayTag가 없을 경우, 해당 태그를 추가하는 함수입니다.
 *
 * 주어진 액터에서 Ability System Component를 가져온 다음, 해당 컴포넌트에
 * 특정 GameplayTag가 없을 경우 태그를 추가합니다.
 *
 * @param InActor   GameplayTag를 검사하고 추가할 대상 액터.
 * @param TagToAdd  대상 액터에 추가하고자 하는 GameplayTag.
 */
void UPOFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	if (!InActor)
	{
		return;
	}

	UPOAbilitySystemComponent* AbilitySystemComponent = NativeGetWarriorASCFromActor(InActor);
	if (!AbilitySystemComponent)
	{
		return;
	}

	// 서버에서만 처리
	if (InActor->HasAuthority())
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(TagToAdd))
		{
			AbilitySystemComponent->AddLooseGameplayTag(TagToAdd);
		}
	}
	else
	{
		// 클라이언트에서 호출 -> 서버에게 요청
		AbilitySystemComponent->ServerAddLooseGameplayTag(TagToAdd);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/PODataAsset_InputConfig.h"
#include "POInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template <class UserObject, typename CallbackFunction>
	void BindNativeInputAction(
		const UPODataAsset_InputConfig* InputConfig, // 입력 액션들이 들어있는 DataAsset
		const FGameplayTag& InputTag, // 바인딩하고 싶은 InputTag (예: Input.Move)
		ETriggerEvent TriggerEvent, // 어떤 입력 트리거에 반응할지 (Pressed, Released 등)
		UserObject* ContextObject, // 바인딩 대상 객체 (예: this)
		CallbackFunction Callback // 바인딩할 함수 포인터 (예: &AMyCharacter::Move)
	);
};

template <class UserObject, typename CallbackFunction>
void UPOInputComponent::BindNativeInputAction(
	const UPODataAsset_InputConfig* InputConfig,
	const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent,
	UserObject* ContextObject,
	CallbackFunction Callback
)
{
	checkf(InputConfig, TEXT("InputConfig DataAsset is null!"));

	if (UInputAction* FoundAction = InputConfig->FindNativeInputActionByTag(InputTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Callback);
	}
}

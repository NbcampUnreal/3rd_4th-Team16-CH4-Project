// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/Input/PODataAsset_InputConfig.h"

#include "POTypes/POStructTypes.h"

UInputAction* UPODataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FPOInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	return nullptr;
}

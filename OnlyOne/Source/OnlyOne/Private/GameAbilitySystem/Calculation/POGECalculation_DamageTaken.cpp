// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Calculation/POGECalculation_DamageTaken.h"
#include "POGameplayTags.h"
#include "GameAbilitySystem/POAttributeSet.h"

struct FPODamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	
	FPODamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPOAttributeSet, BaseDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPOAttributeSet, DamageTaken, Target, false);
	}
};

static FPODamageCapture& GetDamageCapture()
{
	static FPODamageCapture PODamageCapture;
	return PODamageCapture;
}


UPOGECalculation_DamageTaken::UPOGECalculation_DamageTaken()
{
	RelevantAttributesToCapture.Add(GetDamageCapture().BaseDamageDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().DamageTakenDef);
}

void UPOGECalculation_DamageTaken::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// SetByCaller로 전달된 데미지 값을 가져옴. (GA_Attack에서 전달한 값)
	float BaseDamage = Spec.GetSetByCallerMagnitude(POGameplayTags::Shared_SetByCaller_BaseDamage, false, 0.f);
	
	const float FinalDamage = BaseDamage;

	if (FinalDamage > 0.f)
	{
		FGameplayModifierEvaluatedData ModifierEvaluatedData = FGameplayModifierEvaluatedData(
			GetDamageCapture().DamageTakenProperty,
			EGameplayModOp::Override,
			FinalDamage
		);

		// 계산된 최종 데미지를 DamageTaken 속성에 더하는 Modifier를 추가합니다.
		OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
	}
}

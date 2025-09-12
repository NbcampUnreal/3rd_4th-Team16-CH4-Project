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

// TODO: 데미지 계산 처리 로직 작성
void UPOGECalculation_DamageTaken::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	/* 추후 방어력 or 크리티컬 데미지같은 계산이 들어오면 사용예정 현재는 단순 데미지 처리만 처리 */
	// UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	// UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// SetByCaller로 전달된 데미지 값을 가져옵니다. (GA_Attack에서 전달한 값)
	float BaseDamage = Spec.GetSetByCallerMagnitude(POGameplayTags::Shared_SetByCaller_BaseDamage, false, 0.f);

	// 실제 게임에서는 방어력, 크리티컬 등 복잡한 계산이 추가됩니다.
	const float FinalDamage = BaseDamage;

	if (FinalDamage > 0.f)
	{
		// 계산된 최종 데미지를 DamageTaken 속성에 더하는 Modifier를 추가합니다.
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().DamageTakenProperty, EGameplayModOp::Additive, FinalDamage));
	}
}

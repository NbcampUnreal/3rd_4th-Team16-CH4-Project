// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Calculation/POGECalculation_DamageTaken.h"
#include "GameAbilitySystem/POAttributeSet.h"

struct FPODamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	
	FPODamageCapture()
	{
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
	RelevantAttributesToCapture.Add(GetDamageCapture().DamageTakenDef);
}

// TODO: 데미지 계산 처리 로직 작성
void UPOGECalculation_DamageTaken::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	
}

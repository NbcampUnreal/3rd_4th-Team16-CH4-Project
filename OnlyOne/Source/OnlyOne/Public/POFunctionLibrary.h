// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "POTypes/POEnumTypes.h"
#include "POFunctionLibrary.generated.h"

struct FGameplayTag;
class UPOAbilitySystemComponent;
class UPawnCombatComponent;
/**
 * 프로젝트 전반에 걸쳐 블루프린트에서 사용할 수 있는 다양한 전역 유틸리티 함수를 담고 있는 CustomBlueprintFunctionLibrary입니다.
 */
UCLASS()
class ONLYONE_API UPOFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Only One | FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EPOValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "Only One | FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);
};
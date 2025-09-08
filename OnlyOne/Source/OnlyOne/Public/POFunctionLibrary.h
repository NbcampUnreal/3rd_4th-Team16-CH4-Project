// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "POFunctionLibrary.generated.h"

struct FGameplayTag;
class UPOAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPOFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UPOAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);
};

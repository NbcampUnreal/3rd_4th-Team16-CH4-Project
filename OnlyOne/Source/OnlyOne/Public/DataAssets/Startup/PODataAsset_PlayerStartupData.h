// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/Startup/PODataAsset_StartupDataBase.h"
#include "PODataAsset_PlayerStartupData.generated.h"

struct FPOPlayerAbilitySet;
/**
 * 
 */
UCLASS()
class ONLYONE_API UPODataAsset_PlayerStartupData : public UPODataAsset_StartupDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UPOAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FPOPlayerAbilitySet> PlayerStartUpAbilitySets;
	
};

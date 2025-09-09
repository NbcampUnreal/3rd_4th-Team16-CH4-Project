// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/POCharacterBase.h"
#include "PONPCCharacter.generated.h"

class UNpcCombatComponent;
/**
 * 
 */
UCLASS()
class ONLYONE_API APONPCCharacter : public APOCharacterBase
{
	GENERATED_BODY()

public:
	APONPCCharacter();
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	FORCEINLINE UNpcCombatComponent* GetNpcCombatComponent() const { return NpcCombatComponent; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UNpcCombatComponent* NpcCombatComponent;

};

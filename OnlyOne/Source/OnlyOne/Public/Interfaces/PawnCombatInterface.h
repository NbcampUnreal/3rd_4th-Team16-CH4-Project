// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnCombatInterface.generated.h"

class UPawnCombatComponent;

// This class does not need to be modified.
UINTERFACE()
class UPawnCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ONLYONE_API IPawnCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UPawnCombatComponent* GetPawnCombatComponent() const = 0;
};

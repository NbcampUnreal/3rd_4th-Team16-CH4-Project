// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PEInteractManagerHandler.generated.h"

class IPEUseable;
// This class does not need to be modified.
UINTERFACE()
class UPEInteractManagerHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTESCAPE_API IPEInteractManagerHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TryInteract(AActor* TargetActor) = 0;
};

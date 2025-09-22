// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "POInteractManagerHandlerInterface.generated.h"

UINTERFACE()
class UPOInteractManagerHandlerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ONLYONE_API IPOInteractManagerHandlerInterface
{
	GENERATED_BODY()

public:
	virtual void TryInteract(AActor* TargetActor) = 0;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "POUIStackingInterface.generated.h"

class UPOUIStackingComonent;
// This class does not need to be modified.
UINTERFACE()
class UPOUIStackingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ONLYONE_API IPOUIStackingInterface
{
	GENERATED_BODY()

public:
	virtual UPOUIStackingComonent* GetUIStackingComponent() const = 0;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnUIInterface.generated.h"

class UNpcUIComponent;
class UPlayerUIComponent;
class UPawnUIComponent;
// This class does not need to be modified.
UINTERFACE()
class UPawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ONLYONE_API IPawnUIInterface
{
	GENERATED_BODY()

public:
	virtual UPawnUIComponent* GetPawnUIComponent() const = 0;
	virtual UPlayerUIComponent* GetPlayerUIComponent() const;
	virtual UNpcUIComponent* GetNpcUIComponent() const;
};

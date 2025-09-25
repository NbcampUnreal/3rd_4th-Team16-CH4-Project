// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "POInteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPOInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class ONLYONE_API IPOInteractableInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Interactor) = 0;
	virtual bool IsInteractable() const = 0;
	virtual void ShowInteractionUI() = 0;
	virtual void HideInteractionUI() = 0;
};

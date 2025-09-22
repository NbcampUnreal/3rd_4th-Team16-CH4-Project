// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PEInteractable.generated.h"

UINTERFACE(MinimalAPI)
class UPEInteractable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTESCAPE_API IPEInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(class AActor* Interactor) = 0;
	virtual bool IsInteractable() const = 0;
	virtual void ShowInteractionUI() = 0;
	virtual void HideInteractionUI() = 0;
};

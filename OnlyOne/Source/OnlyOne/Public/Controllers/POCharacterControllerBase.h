// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/PawnUIInterface.h"
#include "GenericTeamAgentInterface.h"
#include "POCharacterControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APOCharacterControllerBase : public APlayerController, public IPawnUIInterface , public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
};

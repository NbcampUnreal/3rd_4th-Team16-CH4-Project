// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterControllerBase.h"
#include "PONPCController.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APONPCController : public APOCharacterControllerBase
{
	GENERATED_BODY()

public:
	APONPCController();
	virtual UPawnUIComponent* GetPawnUIComponent() const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNpcUIComponent> NpcUIComponent;
};

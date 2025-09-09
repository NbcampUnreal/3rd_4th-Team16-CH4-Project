// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterControllerBase.h"
#include "POPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APOPlayerController : public APOCharacterControllerBase
{
	GENERATED_BODY()
	
public:
	APOPlayerController();
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UPlayerUIComponent* GetPlayerUIComponent() const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UPlayerUIComponent* PlayerUIComponent;
};

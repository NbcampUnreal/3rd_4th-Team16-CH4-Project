// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "POCharacterBase.generated.h"

class UInputComponent;

UCLASS()
class ONLYONE_API APOCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	APOCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
};

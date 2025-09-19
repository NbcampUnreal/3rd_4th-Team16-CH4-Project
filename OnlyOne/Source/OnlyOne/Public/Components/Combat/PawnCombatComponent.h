// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnCombatComponent.generated.h"

class APOPlayerCharacter;

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	EED_LeftHand,
	EED_RightHand
};

UCLASS()
class ONLYONE_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Only One | Combat")
	void ToggleCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);

	UFUNCTION(BlueprintCallable, Category = "Only One | Combat")
	APOPlayerCharacter* GetOwnerCharacter() const;
	
	virtual void HandleHit(AActor* HitActor);
	
	UFUNCTION(Server, Reliable)
	void ServerToggleCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);


protected:
	// 한 번의 공격 동안 이미 맞은 액터들을 기록하는 배열
	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlappedActors;

private:
	void HandleToggleCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);
};


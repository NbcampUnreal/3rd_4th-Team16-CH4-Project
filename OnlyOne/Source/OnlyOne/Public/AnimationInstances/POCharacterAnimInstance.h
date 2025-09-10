// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterAnimInstance.generated.h"

class APOCharacterBase;
class UCharacterMovementComponent;

UCLASS()
class ONLYONE_API UPOCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetAngle() const { return Angle; }

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0.f; }

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsNotMoving() const { return Speed == 0.f; }

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetIsOnGround() const { return !bIsJumping; }


private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerPlayerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerMovementComponent;

	UPROPERTY(EditAnywhere, Category=Animation)
	float YawSpeedSmoothedLerpSpeed = 1.f;

	float Angle;
	float Speed;
	bool bIsJumping;
	
	FRotator BodyPrevRotation;
	FRotator LookRotationOffset;
};
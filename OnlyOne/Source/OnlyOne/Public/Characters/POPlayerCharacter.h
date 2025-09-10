// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/POCharacterBase.h"
#include "POPlayerCharacter.generated.h"

class UPlayerCombatComponent;
struct FGameplayTag;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UPODataAsset_InputConfig;
/**
 * 
 */
UCLASS()
class ONLYONE_API APOPlayerCharacter : public APOCharacterBase
{
	GENERATED_BODY()

public:
	APOPlayerCharacter();
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera | SpringArm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ViewCamera;

#pragma region Player Movement
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData | Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPODataAsset_InputConfig> InputConfigDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | walk", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Run", meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Run", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 600.f;

	UFUNCTION(Server, Reliable)
	void Server_SetWalking(bool bNewIsWalking);

	UPROPERTY(Replicated)
	bool bIsWalking = false;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Walk(const FInputActionValue& InputActionValue);
	void Input_Sprint(const FInputActionValue& InputActionValue);
	void Input_AbilityInputPressed(const FGameplayTag InInputTag);
	void Input_AbilityInputReleased(const FGameplayTag InInputTag);
	void SetMovementSpeed(const float NewMaxWalkSpeed);
	void UpdateMovementSpeedBasedOnWalkingState(bool bNewIsWalking);

	bool IsInputPressed(const FInputActionValue& InputActionValue);
	bool bIsSprint = false;
	bool bIsJump = false;
	
#pragma endregion

#pragma region Component
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerCombatComponent> PlayerCombatComponent;
#pragma endregion

public:
	FORCEINLINE UPlayerCombatComponent* GetPlayerCombatComponent() const { return PlayerCombatComponent; }
};


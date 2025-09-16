// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/POCharacterBase.h"
#include "POPlayerCharacter.generated.h"

class UBoxComponent;
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
#pragma region Virtual functions
	
	APOPlayerCharacter();
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	virtual void Tick(float DeltaSeconds) override;
	
#pragma endregion
	
protected:
	virtual void PossessedBy(AController* NewController) override;

private:
#pragma region Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera | SpringArm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ViewCamera;
#pragma endregion

#pragma region Movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | walk", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Run", meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Run", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 600.f;

	UPROPERTY(ReplicatedUsing=OnRep_IsWalking)
	bool bIsWalking = false;

	UPROPERTY(ReplicatedUsing=OnRep_IsSprinting)
	bool bIsSprinting = false;

	UFUNCTION(Server, Reliable)
	void Server_SetWalking(bool bNewIsWalking);

	UFUNCTION(Server, Reliable)
	void Server_SetSprinting(bool bNewIsSprinting);

	UFUNCTION()
	void OnRep_IsWalking();

	UFUNCTION()
	void OnRep_IsSprinting();
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Walk(const FInputActionValue& InputActionValue);
	void Input_Sprint(const FInputActionValue& InputActionValue);
	void Input_AbilityInputPressed(const FGameplayTag InInputTag);
	void Input_AbilityInputReleased(const FGameplayTag InInputTag);
	void SetMovementSpeed(const float NewMaxWalkSpeed);
	void UpdateMovementSpeed();

	bool IsInputPressed(const FInputActionValue& InputActionValue);

	bool bIsJump = false;
#pragma endregion
	
#pragma region Hit Collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> AttackHitCollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName AttackHandCollisionBoxAttachBoneName;
	
	UFUNCTION()
	void OnAttackHitBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerCombatComponent> PlayerCombatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData | DataAsset", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPODataAsset_InputConfig> InputConfigDataAsset;

public:
	FORCEINLINE UPlayerCombatComponent* GetPlayerCombatComponent() const { return PlayerCombatComponent; }
	FORCEINLINE UBoxComponent* GetAttackHitCollisionBox() const { return AttackHitCollisionBox; }

};


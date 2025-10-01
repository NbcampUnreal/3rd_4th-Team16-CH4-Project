// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/POCharacterBase.h"
#include "Interfaces/POInteractManagerHandlerInterface.h"
#include "POPlayerCharacter.generated.h"

class UPOInteractManagerComponent;
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
class ONLYONE_API APOPlayerCharacter : public APOCharacterBase, public IPOInteractManagerHandlerInterface
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

	virtual void UnPossessed() override;

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
	void Input_Interaction(const FInputActionValue& InputActionValue);
	void Input_AbilityInputPressed(const FGameplayTag InInputTag);
	void Input_AbilityInputReleased(const FGameplayTag InInputTag);
	void SetMovementSpeed(const float NewMaxWalkSpeed);
	void UpdateMovementSpeed();

	bool IsInputPressed(const FInputActionValue& InputActionValue);

	bool bIsJump = false;

	UFUNCTION()
	void OnSlowTagChanged(const FGameplayTag Tag, int32 NewCount);

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float SlowMultiplier = 0.6f;
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

#pragma region Bind
	bool bSlowTagBind = false;
	FDelegateHandle SlowTagDelegate;

	void UnbindSlowTagDelegate();   //바인드 해제 함수 
#pragma endregion

	/* Interact 섹션 */
public:
	virtual void TryInteract(AActor* TargetActor) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact", meta = (AllowPrivate))
	TObjectPtr<UPOInteractManagerComponent> InteractManagerComponent;

private:
	// 테스트용 상호작용 입력(E 키) 처리 함수
	void Input_TestInteract();

public:
	// 서버가 호출하여 모든 클라이언트에게 몽타주 재생을 지시하는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* Montage);

	// 몽타주를 재생하고, 끝났을 때 특정 동작을 수행하기 위한 함수
	void PlayMontageWithCallback(UAnimMontage* MontageToPlay, const FTimerDelegate& OnMontageEnded);
};

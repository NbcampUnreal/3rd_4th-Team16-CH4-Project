// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationInstances/POCharacterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OnlyOne/OnlyOne.h"

void UPOCharacterAnimInstance::NativeInitializeAnimation()
{
	OwnerPlayerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerPlayerCharacter)
	{
		OwnerMovementComponent = OwnerPlayerCharacter->GetCharacterMovement();
	}
}

void UPOCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwnerPlayerCharacter || !OwnerMovementComponent)
	{
		return ;
	}
	
	const FRotator BodyRotation = OwnerPlayerCharacter->GetActorRotation();
	const FRotator ControlRotation = OwnerPlayerCharacter->GetBaseAimRotation();
	
	const FVector Velocity = OwnerPlayerCharacter->GetVelocity();
	const FRotator MovementRotator = Velocity.ToOrientationRotator(); // 이동 방향
	const FRotator ActorRotator = OwnerPlayerCharacter->GetActorRotation(); // 캐릭터가 보고 있는 방향
	const float YawDiff = FMath::FindDeltaAngleDegrees(ActorRotator.Yaw, MovementRotator.Yaw);
	
	BodyPrevRotation = BodyRotation;
	LookRotationOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, BodyRotation);
	Speed = OwnerPlayerCharacter->GetVelocity().Length();
	bIsJumping = OwnerMovementComponent->IsFalling();
	Angle = YawDiff;
}


void UPOCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

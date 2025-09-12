// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/POPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/Input/POInputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/Combat/PlayerCombatComponent.h"
#include "Camera/CameraComponent.h"
#include "GameAbilitySystem/POAbilitySystemComponent.h"
#include "DataAssets/Startup/PODataAsset_StartupDataBase.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputSubsystems.h"
#include "POGameplayTags.h"

APOPlayerCharacter::APOPlayerCharacter()
{
	bUseControllerRotationPitch = false; // 캐릭터의 Pitch는 고정(또는 코드/모션으로만 변경).
	bUseControllerRotationYaw = false; // 이동 방향으로 캐릭터가 돌고, 시야는 자유롭게 별도 회전
	bUseControllerRotationRoll = false; // 보행 캐릭터는 거의 항상 false. 몸이 좌우로 기울어지는 건 보통 애니메이션이 처리
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 120.f);
	CameraBoom->bUsePawnControlRotation = true;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	PlayerCombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("Player Combat Component"));

	AttackHitCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Hit Collision Box"));
	AttackHitCollisionBox->SetupAttachment(GetRootComponent());
	AttackHitCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APOPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (const APlayerController* OwningPlayerController = GetController<APlayerController>())
	{
		UEnhancedInputLocalPlayerSubsystem* PlayerSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		
		check(PlayerSubsystem);
		
		PlayerSubsystem->RemoveMappingContext(InputConfigDataAsset->DefaultMappingContext);
		PlayerSubsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	}
}


void APOPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UPOInputComponent* POInputComponent = CastChecked<UPOInputComponent>(PlayerInputComponent);

	POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Walk, ETriggerEvent::Triggered, this, &ThisClass::Input_Walk);
	POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Sprint, ETriggerEvent::Triggered, this, &ThisClass::Input_Sprint);
	
	POInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void APOPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APOPlayerCharacter, bIsWalking);
	DOREPLIFETIME(APOPlayerCharacter, bIsSprinting);
}

UPawnCombatComponent* APOPlayerCharacter::GetPawnCombatComponent() const
{
	ensure(PlayerCombatComponent != nullptr);
	return PlayerCombatComponent;
}

void APOPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull())
	{
		if (UPODataAsset_StartupDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			constexpr int32 AbilityApplyLevel = 1;

			LoadedData->GiveToAbilitySystemComponent(POAbilitySystemComponent, AbilityApplyLevel);
		}
	}
}

void APOPlayerCharacter::Server_SetWalking_Implementation(bool bNewIsWalking)
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
	}

	bIsWalking = bNewIsWalking;
	UpdateMovementSpeed();
}

void APOPlayerCharacter::Server_SetSprinting_Implementation(bool bNewIsSprinting)
{
	if (bNewIsSprinting)
	{
		bIsWalking = false;
	}

	bIsSprinting = bNewIsSprinting;
	UpdateMovementSpeed();
}

void APOPlayerCharacter::OnRep_IsWalking()
{
	UpdateMovementSpeed();
}

void APOPlayerCharacter::OnRep_IsSprinting()
{
	UpdateMovementSpeed();
}

void APOPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	
	if (MovementVector.Y != 0.f)
	{
		// 카메라 기준의 '앞 방향'을 계산
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);

		// 그 방향으로 이동시킴
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APOPlayerCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APOPlayerCharacter::Input_Walk(const FInputActionValue& InputActionValue)
{
	if (!IsInputPressed(InputActionValue))
	{
		Server_SetWalking(!bIsWalking);
	}
}

void APOPlayerCharacter::Input_Sprint(const FInputActionValue& InputActionValue)
{
	if (IsInputPressed(InputActionValue))
	{
		Server_SetSprinting(!bIsSprinting);
	}
}

void APOPlayerCharacter::Input_AbilityInputPressed(const FGameplayTag InInputTag)
{
	if (POAbilitySystemComponent)
	{
		POAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
	}
}

void APOPlayerCharacter::Input_AbilityInputReleased(const FGameplayTag InInputTag)
{
	if (POAbilitySystemComponent)
	{
		POAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
	}
}

bool APOPlayerCharacter::IsInputPressed(const FInputActionValue& InputActionValue)
{
	return InputActionValue.Get<bool>();
}

void APOPlayerCharacter::OnAttackHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 자기 자신이나, 이미 죽은 대상은 무시
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	// 어빌리티 시스템에 Gameplay Event 전송
	FGameplayEventData EventPayload;
	EventPayload.Target = OtherActor; // 충돌한 액터를 Target으로 지정

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, POGameplayTags::Event_Attack_Hit, EventPayload);
}

void APOPlayerCharacter::SetMovementSpeed(const float NewMaxWalkSpeed)
{
	if (NewMaxWalkSpeed >= 0.f)
	{
		GetCharacterMovement()->MaxWalkSpeed = NewMaxWalkSpeed;
	}
}

void APOPlayerCharacter::UpdateMovementSpeed()
{
	if (bIsSprinting)
	{
		SetMovementSpeed(SprintSpeed);
	}
	else if (bIsWalking)
	{
		SetMovementSpeed(WalkSpeed);
	}
	else
	{
		SetMovementSpeed(RunSpeed);
	}
}

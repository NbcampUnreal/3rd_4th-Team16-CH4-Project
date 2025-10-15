// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/POPlayerCharacter.h"

#include "DebugHelper.h"
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
#include "POFunctionLibrary.h"
#include "POGameplayTags.h"
#include "Components/Interact/POInteractManagerComponent.h"
#include "InputCoreTypes.h"
#include "Game/POGameInstance.h"

APOPlayerCharacter::APOPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerCombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("Player Combat Component"));

	// 기본 회전 세팅
	bUseControllerRotationPitch = false; // 캐릭터의 Pitch는 고정(또는 코드/모션으로만 변경).
	bUseControllerRotationYaw = false; // 이동 방향으로 캐릭터가 돌고, 시야는 자유롭게 별도 회전
	bUseControllerRotationRoll = false; // 보행 캐릭터는 거의 항상 false. 몸이 좌우로 기울어지는 건 보통 애니메이션이 처리

	// 카메라 세팅
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 120.f);
	CameraBoom->bUsePawnControlRotation = true;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// 이동 세팅
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	// HitCollision 세팅
	AttackHitCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Hit Collision Box"));
	AttackHitCollisionBox->SetupAttachment(GetMesh());

	// Interact Component 세팅
	InteractManagerComponent = CreateDefaultSubobject<UPOInteractManagerComponent>(TEXT("Interact Manager Component"));
	
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
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
	POInputComponent->BindNativeInputAction(InputConfigDataAsset, POGameplayTags::InputTag_Interaction, ETriggerEvent::Triggered, this, &ThisClass::Input_Interaction);
	
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

void APOPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APOPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 어빌리티가 이미 초기화되었다면, 아무것도 하지 않고 함수를 종료
	if (bAbilitiesInitialized)
	{
		return;
	}

	if (!CharacterStartUpData.IsNull())
	{
		if (UPODataAsset_StartupDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			constexpr int32 AbilityApplyLevel = 1;

			LoadedData->GiveToAbilitySystemComponent(POAbilitySystemComponent, AbilityApplyLevel);

			bAbilitiesInitialized = true;
		}
	}
	
	if (POAbilitySystemComponent && !bSlowTagBind)
	{
		// 슬로우 태그 add/remove시 속도 재계산
		SlowTagDelegate =
			POAbilitySystemComponent
			->RegisterGameplayTagEvent(POGameplayTags::Shared_Status_Slow,EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ThisClass::OnSlowTagChanged);

		bSlowTagBind = true;
	}

	// 최초 계산
	UpdateMovementSpeed();
}

void APOPlayerCharacter::UnPossessed()
{
	UnbindSlowTagDelegate();   //UnPossessed되면 바인드 해제 
	Super::UnPossessed();
}

void APOPlayerCharacter::UnbindSlowTagDelegate()
{
	if (POAbilitySystemComponent && SlowTagDelegate.IsValid())
	{
		POAbilitySystemComponent->UnregisterGameplayTagEvent(
			SlowTagDelegate,
			POGameplayTags::Shared_Status_Slow,
			EGameplayTagEventType::NewOrRemoved);

		SlowTagDelegate.Reset();
	}

	bSlowTagBind = false;
}

void APOPlayerCharacter::TryInteract(AActor* TargetActor)
{
	// TODO: 상호작용 시 캐릭터에 애니메이션 재생이 필요하면 추가
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
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		
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

    // GameInstance의 마우스 감도(0~100)를 50 기준 1.0배로 스케일링 (0~2.0배)
    float Scale = 1.f;
    if (const UWorld* World = GetWorld())
    {
        if (const UPOGameInstance* GI = Cast<UPOGameInstance>(World->GetGameInstance()))
        {
            const float Sens01To100 = FMath::Clamp(GI->GetMouseSensitivity(), 0.f, 100.f);
            Scale = FMath::Clamp(Sens01To100 / 50.f, 0.f, 2.f);
        }
    }

    if (LookAxisVector.X != 0.f)
    {
        AddControllerYawInput(LookAxisVector.X * Scale);
    }

    if (LookAxisVector.Y != 0.f)
    {
        AddControllerPitchInput(LookAxisVector.Y * Scale);
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

void APOPlayerCharacter::Input_Interaction(const FInputActionValue& InputActionValue)
{
	if (InteractManagerComponent)
	{
		InteractManagerComponent->TryInteract();
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

void APOPlayerCharacter::OnSlowTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	UpdateMovementSpeed();
}

void APOPlayerCharacter::OnAttackHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor)
	{
		return;
	}

	// 자기 자신은 무시
	if (OtherActor == this)
	{
		return;
	}

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UPOFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			PlayerCombatComponent->HandleHit(OtherActor);
		}
	}
}


void APOPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (AttackHitCollisionBox)
	{
		AttackHitCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APOPlayerCharacter::OnAttackHitBoxOverlap);

		if (!AttackHandCollisionBoxAttachBoneName.IsNone())
		{
			AttackHitCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttackHandCollisionBoxAttachBoneName);
		}
	}
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
	float Speed;

	if (bIsSprinting)
	{
		Speed = SprintSpeed;
	}
	else if (bIsWalking)
	{
		Speed = WalkSpeed;
	}
	else
	{
		Speed = RunSpeed;
	}

	if (POAbilitySystemComponent &&
		POAbilitySystemComponent->HasMatchingGameplayTag(POGameplayTags::Shared_Status_Slow))
	{
		Speed *= SlowMultiplier;  // 슬로우 적용
	}

	SetMovementSpeed(Speed);
}

void APOPlayerCharacter::Input_TestInteract()
{
	if (InteractManagerComponent)
	{
		InteractManagerComponent->TryInteract();
		UE_LOG(LogTemp, Log, TEXT("E Key Pressed - TryInteract called") );
	}
}

void APOPlayerCharacter::PlayMontageWithCallback(UAnimMontage* MontageToPlay, const FTimerDelegate& OnMontageEnded)
{
	if (MontageToPlay)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			// 지정된 몽타주를 재생하고, 재생된 몽타주의 길이(초)를 반환
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay);
			if (MontageLength > 0.f && OnMontageEnded.IsBound())
			{
				// 타이머 핸들을 생성 (타이머 매니저가 관리하는 고유 ID 역할)
				FTimerHandle MontageTimer;

				// 1. 월드의 타이머 매니저를 통해 MontageLength(재생 시간) 후에
				// 2. OnMontageEnded 델리게이트를 호출하도록 타이머를 설정
				// 3. 마지막 false → 반복 실행하지 않고 1회만 실행
				GetWorld()->GetTimerManager().SetTimer(MontageTimer, OnMontageEnded, MontageLength, false);
			}
		}
	}
}

void APOPlayerCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* Montage)
{
	// 로컬 플레이어가 직접 조종하는 캐릭터가 아니라면, 즉시 재생
	if (GetLocalRole() == ROLE_Authority && IsLocallyControlled())
	{
		return;
	}

	PlayMontageWithCallback(Montage, FTimerDelegate());
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Interact/POInteractManagerComponent.h"
#include "Components/Interact/POInteractableComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Interfaces/POInteractManagerHandlerInterface.h"

UPOInteractManagerComponent::UPOInteractManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InteractionRange = 300.0f;
	CurrentInteractable = nullptr;
	OwnerPawn = nullptr;
}

void UPOInteractManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 소유자 Pawn 캐시
	OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("PEInteractManagerComponent: Owner is not a Pawn!"));
		return;
	}
}

void UPOInteractManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Todo: 상호작용 가능 UI 호출
	CheckInteractableUnderRay();
}

void UPOInteractManagerComponent::TryInteract()
{
	// 상호작용 시도 시 Ray 캐스팅 실행
	CheckAndSetForInteractable();
	
	if (CurrentInteractable)
	{
		// 상호작용 실행
		if (CurrentInteractable->Interact(OwnerPawn))
		{
			if (IPOInteractManagerHandlerInterface* TryInteractInterface = Cast<IPOInteractManagerHandlerInterface>(OwnerPawn))
			{
				TryInteractInterface->TryInteract(CurrentInteractable->GetOwner());
				UE_LOG(LogTemp, Warning, TEXT("Interacted with: %s"), *CurrentInteractable->GetOwner()->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Owner Pawn does not implement IPETryInteract interface!"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No interactable object found"));
	}
}

void UPOInteractManagerComponent::CheckAndSetForInteractable()
{
	if (!OwnerPawn)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 카메라 위치와 방향 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	
	// Pawn의 Control Rotation 사용 (화면 중앙 방향)
	// NOTE: 액터 카메라가 아닌 Pawn의 특정 부분이 사용되는 것 같음 수정 필요
	OwnerPawn->GetActorEyesViewPoint(CameraLocation, CameraRotation);
	
	// Ray 시작점과 끝점 계산
	FVector StartLocation = CameraLocation;
	FVector ForwardVector = CameraRotation.Vector();
	FVector EndLocation = StartLocation + (ForwardVector * InteractionRange);

	// Ray Cast 설정
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn);
	QueryParams.bTraceComplex = false;
	
	FHitResult HitResult;
	bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		//CCHANNEL_INTERACT ,
		ECC_Visibility, //TODO: 프로젝트에 맞는 채널로 수정 필요
		QueryParams
	);

	// 디버그 라인 그리기 (개발 중에만)
#if UE_BUILD_DEBUG
	DrawDebugLine(
		World,
		StartLocation,
		EndLocation,
		bHit ? FColor::Green : FColor::Red,
		false,
		0.1f,
		0,
		1.0f
	);
#endif

	// 이전 상호작용 대상 초기화
	UPOInteractableComponent* NewInteractable = nullptr;

	if (bHit && HitResult.GetActor())
	{
		// Hit된 액터에서 PEInteractableComponent 찾기
		NewInteractable = HitResult.GetActor()->FindComponentByClass<UPOInteractableComponent>();
	}

	// 상호작용 대상이 변경되었을 때 로그 출력
	if (CurrentInteractable != NewInteractable)
	{
		if (NewInteractable)
		{
			UE_LOG(LogTemp, Log, TEXT("Found interactable: %s"), *NewInteractable->GetOwner()->GetName());
		}
		else if (CurrentInteractable)
		{
			UE_LOG(LogTemp, Log, TEXT("Lost interactable: %s"), *CurrentInteractable->GetOwner()->GetName());
		}
		
		CurrentInteractable = NewInteractable;
	}
}

void UPOInteractManagerComponent::OnInteractPressed(const FInputActionValue& Value)
{
	// 상호작용 실행
	TryInteract();
}

void UPOInteractManagerComponent::CheckInteractableUnderRay()
{
	/* 상호작용을 위한 Ray 발사 섹션 */
	// 카메라 위치와 방향 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;

	// Pawn의 Control Rotation 사용 (화면 중앙 방향)
	// NOTE: 액터 카메라가 아닌 Pawn의 특정 부분이 사용되는 것 같음 수정 필요
	OwnerPawn->GetActorEyesViewPoint(CameraLocation, CameraRotation);

	// Ray 시작점과 끝점 계산
	FVector StartLocation = CameraLocation;
	FVector ForwardVector = CameraRotation.Vector();
	FVector EndLocation = StartLocation + (ForwardVector * InteractionRange);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility /*수정 필요*/, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (UPOInteractableComponent* InteractableComp = HitActor->FindComponentByClass<UPOInteractableComponent>())
			{
				// 이전 대상과 다르면 전 대상은 끄기
				if (LastHighlightedComp && LastHighlightedComp != InteractableComp)
				{
					LastHighlightedComp->Highlight(false);
				}

				// 새 대상 Highlight 켜기
				InteractableComp->Highlight(true);
				LastHighlightedComp = InteractableComp;
				return;
			}
		}
	}

	// 부딪힌게 없거나 상호작용 대상이 아니면
	if (LastHighlightedComp)
	{
		LastHighlightedComp->Highlight(false);
		LastHighlightedComp = nullptr;
	}

}

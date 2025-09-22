// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Components/PEInteractableComponent.h"
#include "Items/Interface/PEInteractable.h"
#include "Components/WidgetComponent.h"
#include "Components/MeshComponent.h"

UPEInteractableComponent::UPEInteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	ComponentOwner = nullptr;

	// Collision 설정
	SetCollisionProfileName(TEXT("InteractBox"));
	SetCollisionObjectType(ECC_GameTraceChannel5); // CCHANNEL_INTERACTABLE
	
	SetRenderCustomDepth(false);
	SetCustomDepthStencilValue(0);
}

void UPEInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 소유자 액터가 IPEInteractable 인터페이스를 구현하는지 확인
	SetComponentOwnerInterface(GetOwner());

	if (AActor* OwnerActor = GetOwner())
	{
		// 폴백: 메시 직접 찾기
		CachedMesh = OwnerActor->FindComponentByClass<UMeshComponent>();
	}
  
	// 하이라이트 초기 설정
	Highlight(false);
}

bool UPEInteractableComponent::Interact(AActor* Interactor)
{
	if (!ComponentOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("PEInteractableComponent: ComponentOwner is null!"));
		return false;
	}
	
	if (Interactor)
	{
		if (!ComponentOwner->IsInteractable())
		{
			return false;
		}
		ComponentOwner->Interact(Interactor);
		UE_LOG(LogTemp, Log, TEXT("Interact with %s by %s"), *GetOwner()->GetName(), *Interactor->GetName());

	}
	return true;
}

void UPEInteractableComponent::SetComponentOwnerInterface(UObject* NewOwner)
{
	if (NewOwner && NewOwner->Implements<UPEInteractable>())
	{
		ComponentOwner.SetObject(NewOwner);
		ComponentOwner.SetInterface(Cast<IPEInteractable>(NewOwner));
	}
	else
	{
		ComponentOwner.SetObject(nullptr);
		ComponentOwner.SetInterface(nullptr);
	}
}

void UPEInteractableComponent::Highlight(bool bIsEnable)
{
	if (AActor* Owner = GetOwner())
	{
		if (IPEInteractable* InteractableItem = Cast<IPEInteractable>(Owner))
		{
			if (bIsEnable)
			{
				InteractableItem->ShowInteractionUI();
			}
			else
			{
				InteractableItem->HideInteractionUI();
			}
		}
	}

	if (CachedMesh)
	{
		CachedMesh->SetRenderCustomDepth(bIsEnable);
		// 필요 시 색상 지정 (Material에서 이 StencilValue를 기준으로 색상 분기)
		CachedMesh->SetCustomDepthStencilValue(bIsEnable ? 1 : 0);
	}
}

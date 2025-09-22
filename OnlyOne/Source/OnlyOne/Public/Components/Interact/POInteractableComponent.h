// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "POInteractableComponent.generated.h"


class IPOInteractableInterface;
class UBoxComponent;
class UWidgetComponent;

/**
 * UPEInteractableComponent는 상호작용 가능한 액터에 부착되어 상호작용 기능을 제공하는 컴포넌트입니다.
 * 이 컴포넌트는 IPEInteractable 인터페이스를 구현하여 상호작용 기능을 제공합니다.
 *
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ONLYONE_API UPOInteractableComponent : public UBoxComponent
{
	GENERATED_BODY()

	/* Lifecycle 관련 섹션 */
public:	
	UPOInteractableComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	/* Interact 관련 섹션 */

public:	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool Interact(AActor* Interactor);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TScriptInterface<IPOInteractableInterface> ComponentOwner;

	void SetComponentOwnerInterface(UObject* NewOwner);

	/* 하이라이트 관련 섹션 */
public:
	void Highlight(bool bIsEnable);

private:
	UPROPERTY()
	TObjectPtr<UMeshComponent> CachedMesh; // 대상 Mesh 보관용

	UPROPERTY()
	TObjectPtr<USceneComponent> RootComp; // 루트 컴포넌트 가져오기
	
};

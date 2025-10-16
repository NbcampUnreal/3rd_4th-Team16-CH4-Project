// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Engine.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "POInteractManagerComponent.generated.h"

class UPOInteractableComponent;
class UInputAction;

/*
 *	Interact를 실행시킬 액터에 부착하여 사용하는 컴포넌트 클래스입니다.
 *	구현해야 하는 기능:
 *	1. 상호작용 가능한 액터를 Ray 캐스팅으로 감지하는 기능
 *	2. 상호작용 가능한 액터와의 거리를 계산하여 상호작용 가능한지 여부를 판단하는 기능
 *	3. 상호작용 입력을 처리하는 기능
 *	4. 상호작용 가능한 액터와의 상호작용을 실행하는 기능
 *
 *	Todo: 현재 입력을 해당 컴포넌트에서 받고 있음,
 *		해당 컴포넌트의 오너가 입력을 처리하고 컴포넌트의 상호작용 기능을 호출하도록 변경해야함
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONLYONE_API UPOInteractManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	/* Lifecycle 관련 섹션 */
public:
	UPOInteractManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/* Interact 관련 섹션 */
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 300.0f;
	
	UPROPERTY()
	TObjectPtr<UPOInteractableComponent> CurrentInteractable;

	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CheckAndSetForInteractable();

	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool HasInteractable() const { return CurrentInteractable != nullptr; }

	// 서버 검증용 상호작용 요청 (클라이언트가 카메라 정보 전달)
	UFUNCTION(Server, Reliable)
	void ServerTryInteract(FVector_NetQuantize Start, FVector_NetQuantizeNormal Direction);

protected:
	// 상호작용 입력 처리 함수
	void OnInteractPressed(const FInputActionValue& Value);

	/* Interact 하이라이트 관련 섹션 */
protected:
	UPROPERTY()
	TObjectPtr<UPOInteractableComponent> LastHighlightedComp;

public:
	void CheckInteractableUnderRay();
};

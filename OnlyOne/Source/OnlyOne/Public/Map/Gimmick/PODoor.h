#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/POInteractableInterface.h"
#include "Components/Interact/POInteractableComponent.h"
#include "PODoor.generated.h"

UCLASS()
class ONLYONE_API APODoor : public AActor, public IPOInteractableInterface
{
	GENERATED_BODY()
	
public:	
	APODoor();

protected:
#pragma region Components
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Door")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Interact")
	TObjectPtr<UPOInteractableComponent> InteractableComp;


#pragma endregion

#pragma region New 
	UPROPERTY(ReplicatedUsing=OnRep_Holding)
	bool bHolding = false;
	UFUNCTION()
	void OnRep_Holding(){}

	//  추가: 대기 시간
	UPROPERTY(EditAnywhere, Category="Hold")
	float HoldDuration = 0.1f;

	// 타이머 핸들/완료 콜백
	FTimerHandle HoldTimer;
	void OnHoldComplete(AActor* InstigatorActor);

	//  서버에서 시작하도록 RPC
	UFUNCTION(Server, Reliable)
	void ServerStartHold(AActor* InstigatorActor);

	// 내부 진입점
	void StartHold(AActor* InstigatorActor);

	// 자동 닫힘 옵션 & 타이머
	UPROPERTY(EditAnywhere, Category="Door|AutoClose")
	bool bAutoCloseAfterOpen = true;

	UPROPERTY(EditAnywhere, Category="Door|AutoClose", meta=(EditCondition="bAutoCloseAfterOpen", ClampMin="0.0"))
	float AutoCloseDelay = 3.0f;

	FTimerHandle AutoCloseTimer;
	
#pragma endregion
	UPROPERTY(ReplicatedUsing=OnRep_IsOpen, BlueprintReadOnly, Category="Door")
	bool bIsOpen = false;

	
	UFUNCTION()
	void OnRep_IsOpen();

	UFUNCTION(BlueprintImplementableEvent, Category="Door|BP")
	void PlayOpen();
	UFUNCTION(BlueprintImplementableEvent, Category="Door|BP")
	void PlayClose();

#pragma region Server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerToggleDoor(AActor* InstigatorActor);
private:
	void PerformToggleDoor(AActor* InstigatorActor);
	void SetDoorState(bool bOpen, AActor* InstigatorActor);
#pragma endregion
	
#pragma region Interact
public:
	virtual void Interact(AActor* Interactor) override;
	virtual bool IsInteractable() const override;
	virtual void ShowInteractionUI() override;
	virtual void HideInteractionUI()override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InteractWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> InteractWidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float InteractionUIOffsetZ = 50.0f;
	
#pragma endregion 

public:
	bool IsOpen() const { return bIsOpen; }
};

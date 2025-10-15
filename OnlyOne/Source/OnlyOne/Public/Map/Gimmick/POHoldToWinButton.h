#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/POInteractableInterface.h"
#include "Components/Interact/POInteractableComponent.h"
#include "game/POStageGameMode.h"
#include "Components/BoxComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "POHoldToWinButton.generated.h"

UCLASS()
class ONLYONE_API APOHoldToWinButton : public AActor , public IPOInteractableInterface
{
	GENERATED_BODY()
	
public:	
	
	APOHoldToWinButton();

	
protected:
	virtual void BeginPlay() override;
#pragma region Components
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	TObjectPtr<UStaticMeshComponent> Button;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	TObjectPtr<UPOInteractableComponent> InteractableComp;

#pragma endregion
	UPROPERTY(VisibleAnywhere,category="Hold")
	float HoldDuration = 10.0f;

	UPROPERTY(ReplicatedUsing=OnRep_Holding)
	bool bHolding = false;

	UPROPERTY(EditAnywhere, Category="HoldToWin|Movement")
	float MoveCancelRadius = 80.f;

	// 이동/상태 체크 주기
	UPROPERTY(EditAnywhere, Category="HoldToWin|Movement")
	float MoveCheckInterval = 0.05f;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> HolderAsc;
	FDelegateHandle DeadAbilityTagHandle;
	
	UFUNCTION()
	void OnRep_Holding();

	UFUNCTION(BlueprintImplementableEvent)
	void StartPress();
	UFUNCTION(BlueprintImplementableEvent)
	void EndPress();

	void TickHoldCheck();
	
	UFUNCTION()
	void OnDeathTagChanged(const FGameplayTag Tag, int32 NewCount);
	
private:

	FTimerHandle HoldTimer;
	FTimerHandle MoveCheckTimer;
	
	void StartHold(APawn* InPawn);
	void CancelHold();
	void EndHold();
	
	

#pragma region Server
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerStartHold(APawn* InPawn);

private:
	UPROPERTY(Replicated)
	bool bHoldButton = false;
	
	UPROPERTY()
	APawn* HolderPawn = nullptr;
	UPROPERTY()
	FVector AnchorLocation = FVector::ZeroVector;

	
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
	
};

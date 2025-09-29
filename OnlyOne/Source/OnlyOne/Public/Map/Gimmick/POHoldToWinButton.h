#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/POInteractableInterface.h"
#include "Components/Interact/POInteractableComponent.h"
#include "game/POStageGameMode.h"
#include "Components/BoxComponent.h"
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

	UFUNCTION()
	void OnRep_Holding();

	UFUNCTION(BlueprintImplementableEvent)
	void StartPress();
	UFUNCTION(BlueprintImplementableEvent)
	void EndPress();
	
private:

	FTimerHandle HoldTimer;
	
	void StartHold(APawn* InPawn);
	void CancelHold();
	void EndHold();
	
	

#pragma region Server
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	bool bHoldButton = false;
	
	UFUNCTION(Server, Reliable)
	void ServerStartHold(APawn* InPawn);

	UPROPERTY()
	APawn* HolderPawn = nullptr;

	
#pragma endregion



#pragma region Interact
public:
	virtual void Interact(AActor* Interactor) override;
	virtual bool IsInteractable() const override;
	virtual void ShowInteractionUI() override;
	virtual void HideInteractionUI()override;
#pragma endregion 
	
};

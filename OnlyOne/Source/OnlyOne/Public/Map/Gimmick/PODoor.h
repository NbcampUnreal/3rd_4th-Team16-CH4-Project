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
	UFUNCTION()
	void ServerTestAutoToggle();
	
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
#pragma endregion 

	

};

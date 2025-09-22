#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PODoor.generated.h"


UCLASS()
class ONLYONE_API APODoor : public AActor
{
	GENERATED_BODY()
	
public:	
	APODoor();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Door")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	
	UPROPERTY(ReplicatedUsing=ORep_IsOpen, BlueprintReadOnly, Category="Door")
	bool bIsOpen = false;
	
	UFUNCTION()
	void ORep_IsOpen();

	UFUNCTION(BlueprintImplementableEvent, Category="Door|BP")
	void PlayOpen();
	UFUNCTION(BlueprintImplementableEvent, Category="Door|BP")
	void PlayClose();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "NLP_PODoor.generated.h"

class APODoor;

UCLASS()
class ONLYONE_API ANLP_PODoor : public ANavLinkProxy
{
	GENERATED_BODY()
	
public:
	ANLP_PODoor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OpenDoor(AActor* MovingActor, const FVector& DestinationPoint);

	void ResumeNPC(AActor* MovingActor, const FVector& DestinationPoint);

	UPROPERTY(EditAnywhere, Category = "Door")
	TObjectPtr<APODoor> TargetDoorActor;
};

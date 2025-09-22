// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterControllerBase.h"
#include "POPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APOPlayerController : public APOCharacterControllerBase
{
	GENERATED_BODY()
	
public:
	APOPlayerController();
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerUIComponent> PlayerUIComponent;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	/* UI 섹션 */
public:
};

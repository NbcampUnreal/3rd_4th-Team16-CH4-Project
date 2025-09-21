// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "POStageGameState.generated.h"


UENUM(BlueprintType)
enum class EPOStagePhase : uint8
{
	Prep	UMETA(DisplayName="Prep"),
	Active	UMETA(DisplayName="Active")
};

/**
 * 
 */
UCLASS()
class ONLYONE_API APOStageGameState : public AGameState
{
	GENERATED_BODY()

public:
	APOStageGameState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UFUNCTION() void OnRep_Phase();
	UFUNCTION() void OnRep_PrepRemainingSeconds();

	void TickPrepCountdown();

public:
	UFUNCTION(BlueprintPure, Category="PO|Phase")
	EPOStagePhase GetPhase() const { return Phase; }

	UFUNCTION(BlueprintPure, Category="PO|Phase")
	int32 GetPrepRemainingSeconds() const { return PrepRemainingSeconds; }
	
	UFUNCTION(Category="PO|Phase")
	void ServerStartPrepCountdown(int32 InSeconds);

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPhaseChanged, EPOStagePhase);
	FPOOnPhaseChanged OnPhaseChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPrepTimeUpdated, int32);
	FPOOnPrepTimeUpdated OnPrepTimeUpdated;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_Phase)
	EPOStagePhase Phase;

	UPROPERTY(ReplicatedUsing=OnRep_PrepRemainingSeconds)
	int32 PrepRemainingSeconds;

private:
	FTimerHandle PrepTimerHandle;
};

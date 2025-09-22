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

	/* Phase */
	UFUNCTION(BlueprintPure, Category="PO|Phase")
	EPOStagePhase GetPhase() const { return Phase; }

	UFUNCTION(BlueprintPure, Category="PO|Phase")
	int32 GetPrepRemainingSeconds() const { return PrepRemainingSeconds; }

	UFUNCTION(Category="PO|Phase")
	void ServerStartPrepCountdown(int32 InSeconds);

	/* Stage Time */
	UFUNCTION(BlueprintPure, Category="PO|StageTime")
	int32 GetStageRemainingSeconds() const { return StageRemainingSeconds; }

	UFUNCTION(Category="PO|StageTime")
	void ServerStartStageCountdown(int32 InSeconds);

	/* Delegates */
	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPhaseChanged, EPOStagePhase);
	FPOOnPhaseChanged OnPhaseChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPrepTimeUpdated, int32);
	FPOOnPrepTimeUpdated OnPrepTimeUpdated;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnStageTimeUpdated, int32);
	FPOOnStageTimeUpdated OnStageTimeUpdated;

	DECLARE_MULTICAST_DELEGATE(FPOOnStageTimeExpired);
	FPOOnStageTimeExpired OnStageTimeExpired;

	/* ===== protected: Unreal Lifecycle & RepNotify ===== */
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnRep_Phase();

	UFUNCTION()
	void OnRep_PrepRemainingSeconds();

	UFUNCTION()
	void OnRep_StageRemainingSeconds();

	/* ===== protected: Replicated State ===== */
protected:
	UPROPERTY(ReplicatedUsing=OnRep_Phase)
	EPOStagePhase Phase;

	UPROPERTY(ReplicatedUsing=OnRep_PrepRemainingSeconds)
	int32 PrepRemainingSeconds;

	UPROPERTY(ReplicatedUsing=OnRep_StageRemainingSeconds)
	int32 StageRemainingSeconds = 0;

	/* ===== private: Timers & Internal Ticks ===== */
private:
	FTimerHandle PrepTimerHandle;
	FTimerHandle StageTimerHandle;

	void TickPrepCountdown();
	void TickStageCountdown();
};
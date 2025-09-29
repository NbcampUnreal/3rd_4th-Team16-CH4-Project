// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "POStageGameState.generated.h"


UENUM(BlueprintType)
enum class EPOStagePhase : uint8
{
	Prep	UMETA(DisplayName="Prep"),
	Active	UMETA(DisplayName="Active"),
	RoundEnd UMETA(DisplayName="RoundEnd"),
	GameEnd  UMETA(DisplayName="GameEnd")
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

	UFUNCTION(Server, Reliable, Category="PO|Phase")
	void ServerSetPhase(EPOStagePhase NewPhase); 

	UFUNCTION(BlueprintPure, Category="PO|Phase")
	int32 GetPrepRemainingSeconds() const { return PrepRemainingSeconds; }

	UFUNCTION(Category="PO|Phase")
	void ServerStartPrepCountdown(int32 InSeconds);

	/* Stage Time */
	UFUNCTION(BlueprintPure, Category="PO|StageTime")
	int32 GetStageRemainingSeconds() const { return StageRemainingSeconds; }

	UFUNCTION(Category="PO|StageTime")
	void ServerStartStageCountdown(int32 InSeconds);

	/* Winner */
	UFUNCTION(BlueprintPure, Category="PO|Result")
	APlayerState* GetWinnerPlayerState() const { return WinnerPS; }

	UFUNCTION(Server, Reliable, Category="PO|Result")
	void ServerSetWinner(APlayerState* InWinner);
	
	UFUNCTION(Server, Reliable, Category="PO|Result")
	void ServerClearWinner(); 

	/* Delegates */
	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPhaseChanged, EPOStagePhase);
	FPOOnPhaseChanged OnPhaseChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPrepTimeUpdated, int32);
	FPOOnPrepTimeUpdated OnPrepTimeUpdated;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnStageTimeUpdated, int32);
	FPOOnStageTimeUpdated OnStageTimeUpdated;

	DECLARE_MULTICAST_DELEGATE(FPOOnStageTimeExpired);
	FPOOnStageTimeExpired OnStageTimeExpired;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnWinnerDecided, APlayerState*);
	FPOOnWinnerDecided OnWinnerDecided;

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

	UFUNCTION()
	void OnRep_WinnerPS();

	/* ===== protected: Replicated State ===== */
protected:
	UPROPERTY(ReplicatedUsing=OnRep_Phase)
	EPOStagePhase Phase;

	UPROPERTY(ReplicatedUsing=OnRep_PrepRemainingSeconds)
	int32 PrepRemainingSeconds;

	UPROPERTY(ReplicatedUsing=OnRep_StageRemainingSeconds)
	int32 StageRemainingSeconds = 0;

	UPROPERTY(ReplicatedUsing=OnRep_WinnerPS)
	APlayerState* WinnerPS = nullptr;

	/* ===== private: Timers & Internal Ticks ===== */
private:
	FTimerHandle PrepTimerHandle;
	FTimerHandle StageTimerHandle;

	void TickPrepCountdown();
	void TickStageCountdown();
};
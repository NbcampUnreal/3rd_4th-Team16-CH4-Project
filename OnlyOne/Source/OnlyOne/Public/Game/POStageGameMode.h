// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POStageGameState.h"
#include "GameFramework/GameMode.h"
#include "POStageGameMode.generated.h"

class APOLobbyPlayerState;

/**
 * 
 */
UCLASS()
class ONLYONE_API APOStageGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APOStageGameMode();

	/* Spawn/Phase Config */
	UPROPERTY(EditDefaultsOnly, Category="PO|Spawn|Player")
	TSubclassOf<APawn> PlayerPawnClass;

	UPROPERTY(EditDefaultsOnly, Category="PO|Spawn|AI")
	TSubclassOf<APawn> AIClass;

	UPROPERTY(EditDefaultsOnly, Category="PO|Spawn", meta=(ClampMin="0", ClampMax="64"))
	int32 NumAIsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category="PO|Spawn", meta=(ClampMin="200.0"))
	float AIRandomRadius;

	UPROPERTY(EditDefaultsOnly, Category="PO|Phase", meta=(ClampMin="0", ClampMax="60"))
	int32 PrepSeconds;

	UPROPERTY(EditDefaultsOnly, Category="PO|Phase", meta=(ClampMin="0", ClampMax="3600"))
	int32 StageSeconds;

	/* ===== protected: Unreal Lifecycle ===== */
protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;

	/* ===== protected: Game Rules & Flow ===== */
protected:
	void CachePlayerStarts();
	void SpawnAIsOnStage();
	void HandlePhaseChanged(EPOStagePhase NewPhase);
	void ResetAllPlayerStatesForMatchStart();

	/* ===== private: Internal State ===== */
private:
	TArray<TWeakObjectPtr<AActor>> CachedPlayerStarts;
	TSet<TWeakObjectPtr<AActor>>   UsedPlayerStarts;

	FCriticalSection StartLock;
	bool bDidSubscribePhase = false;

	bool IsSpawnPointFree(AActor* SpawnPoint) const;
};
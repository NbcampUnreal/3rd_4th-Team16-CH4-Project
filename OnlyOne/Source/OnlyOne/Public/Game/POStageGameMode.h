// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POGameMode.h"
#include "POStageGameState.h"
#include "POStageGameMode.generated.h"

class APOLobbyPlayerState;

/**
 * 
 */
UCLASS()
class ONLYONE_API APOStageGameMode : public APOGameMode
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
	
	UPROPERTY(EditDefaultsOnly, Category="PO|Flow", meta=(ClampMin="0", ClampMax="60"))
	int32 GameEndReturnSeconds = 5;
	
	UPROPERTY(EditDefaultsOnly, Category="PO|Flow")
	FString LobbyMapURL = TEXT("/Game/Maps/Lobby?listen");
	
	UFUNCTION(BlueprintCallable, Category="PO|Rules")
	void NotifyCharacterDied(AController* VictimController, AController* KillerController);

	UFUNCTION(BlueprintCallable)
	void EndGameForGimmick(APlayerState* WinnerPS);
	
	/* ===== protected: Unreal Lifecycle ===== */
protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	

	/* ===== protected: Game Rules & Flow ===== */
protected:
	TSet<TWeakObjectPtr<APlayerState>> AlivePlayers;
	void CachePlayerStarts();
	void SpawnAIsOnStage();
	void HandlePhaseChanged(EPOStagePhase NewPhase);
	void HandleStageTimeExpired();
	void ResetAllPlayerStatesForMatchStart();
	void BeginRoundEndPhase();
	void BeginGameEndPhase(APlayerState* InWinnerPS);
	void CompactAlivePlayers();
	void TryDecideWinner();
	void NotifySpecialVictory(APlayerState* WinnerPS);
	void WipeAllAIsOnStage();
	bool ShouldEnterSpectatorOnJoin() const;
	void EnterSpectatorForMidJoin(APlayerController* PC);
	void StartReturnToLobbyTimer();
	void DoReturnToLobby();  

	/* ===== private: Internal State ===== */
private:
	TArray<TWeakObjectPtr<AActor>> CachedPlayerStarts;
	TSet<TWeakObjectPtr<AActor>>   UsedPlayerStarts;

	bool bDidSubscribePhase = false;
	bool bGameEndStarted = false;
	bool bAIWipedAtRoundEnd = false;

	FTimerHandle ReturnToLobbyTimerHandle;

	bool IsSpawnPointFree(AActor* SpawnPoint) const;
	APOLobbyPlayerState* ToLobbyPS(AController* C) const;
	APOLobbyPlayerState* ToLobbyPS(AActor* A) const;

public:
	UPROPERTY(EditDefaultsOnly, Category="PO|Stage", meta=(ClampMin="2", ClampMax="16", UIMin="2", UIMax="16")) // [ADDED]
	int32 MaxPlayersInStage = 8;
};
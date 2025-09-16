// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "POLobbyGameMode.generated.h"

class APOLobbyPlayerState;
class APOLobbyGameState;

/**
 * 
 */
UCLASS()
class ONLYONE_API APOLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APOLobbyGameMode();
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

public:
	void NotifyReadyStateChanged(APOLobbyPlayerState* ChangedPlayer, bool bNowReady);

protected:
	bool AreAllPlayersReady() const;
	
	void TryStartCountdown();
	void CancelCountdown(const TCHAR* Reason);
	void TickCountdown();
	void OnCountdownFinished();
	
protected:
	FTimerHandle StartMatchTimerHandle;
	int32 CountdownRemaining;

	UPROPERTY(EditDefaultsOnly, Category="PO|Lobby", meta=(ClampMin="2", ClampMax="16", UIMin="2", UIMax="16"))
	int32 MaxPlayersInLobby;

	UPROPERTY(EditDefaultsOnly, Category="PO|Lobby", meta=(ClampMin="1", ClampMax="60", UIMin="1", UIMax="30"))
	int32 CountdownSecondsDefault;

	bool bCountdownRunning;
	
};

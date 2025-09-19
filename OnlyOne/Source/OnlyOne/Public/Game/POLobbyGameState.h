// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "POLobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API APOLobbyGameState : public AGameState
{
	GENERATED_BODY()

public:
	APOLobbyGameState();

public:
	int32 GetCountdownRemaining() const { return CountdownRemaining; }
	bool IsAllReady() const { return bAllReady; }
	
	void SetCountdownRemaining(int32 InRemaining);
	void SetAllReady(bool bInAllReady);

protected:
	UPROPERTY(Replicated)
	int32 CountdownRemaining;

	UPROPERTY(Replicated)
	bool bAllReady;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
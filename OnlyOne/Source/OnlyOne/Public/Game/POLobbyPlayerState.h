// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "POLobbyPlayerState.generated.h"

/**
 * 
 */	
UCLASS()
class ONLYONE_API APOLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APOLobbyPlayerState();

	/* Lobby */
	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void InitNicknameFromGameInstanceOnce();

	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void ToggleReady();

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	bool IsReady() const { return bIsReady; }

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetBaseNickname() const { return BaseNickname; } // NOTE: Getter는 FORCEINLINE 권장

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetDisplayNickname() const { return DisplayNickname; }

	/* Stats / Match */
	UFUNCTION(BlueprintPure, Category="PO|Stats")
	int32 GetKillScore() const { return KillScore; }

	UFUNCTION(BlueprintPure, Category="PO|Stats")
	bool IsAlive() const { return bIsAlive; }

	UFUNCTION()
	void ServerResetForMatchStart();

	/* Server RPCs */
	UFUNCTION(Server, Reliable)
	void ServerSetNicknameOnce(const FString& InNickname);

	UFUNCTION(Server, Reliable)
	void ServerSetReady();

	UFUNCTION(Server, Reliable)
	void ServerAddKill(int32 Delta = 1);

	UFUNCTION(Server, Reliable)
	void ServerSetAlive(bool bInAlive);

	/* Notifications (Multicast) */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerJoinedLobby(const FString& InName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerLeftLobby(const FString& InName);

	/* Server-only */
public:
	UFUNCTION(Category="PO|Stats")
	void AddKill_ServerOnly(int32 Delta = 1);
	
	UFUNCTION(Category="PO|Stats")
	void SetAlive_ServerOnly(bool bInAlive);

protected:
	/* UE Lifecycle */
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* RepNotifies */
	UFUNCTION()
	void OnRep_IsReady();

	UFUNCTION()
	void OnRep_KillScore();

	UFUNCTION()
	void OnRep_IsAlive();

protected:
	/* Lobby state */
	UPROPERTY(ReplicatedUsing=OnRep_IsReady)
	bool bIsReady = false;

	UPROPERTY(Replicated)
	FString BaseNickname;

	UPROPERTY(Replicated)
	FString DisplayNickname;

	/* Stage Stats */
	UPROPERTY(ReplicatedUsing=OnRep_KillScore)
	int32 KillScore = 0;

	UPROPERTY(ReplicatedUsing=OnRep_IsAlive)
	bool bIsAlive = true;

private:
	void InitializeExistingPlayers();
	
	FString SanitizeNickname_Server(const FString& InRaw) const;
};
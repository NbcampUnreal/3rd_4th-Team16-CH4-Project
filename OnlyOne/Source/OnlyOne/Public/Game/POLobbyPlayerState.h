// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "POLobbyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOOnReadyChanged, bool, bNowReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitalizedPlayerData, const FJoinServerData&, PlayerData);

/**
 * 
 */
UCLASS()
class ONLYONE_API APOLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

	/* Lifecycle 섹션 */

public:
	APOLobbyPlayerState();
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Lobby 관련 섹션 */
public:
	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void InitNicknameFromGameInstanceOnce();
	
	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	bool IsReady() const { return bIsReady; }

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetBaseNickname() const { return BaseNickname; } //NOTE: Getter는 FORCEINLINE을 붙이는게 좋음

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetDisplayNickname() const { return DisplayNickname; }
	
	UPROPERTY(BlueprintAssignable, Category="PO|Lobby")
	FPOOnReadyChanged OnReadyChanged;

	UPROPERTY(BlueprintAssignable, Category="PO|Lobby")
	FOnInitalizedPlayerData OnInitalizedPlayerData;

	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void ToggleReady();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerLeftLobby(const FString& InName);
	
	void InitializeExistingPlayers();

public:
	UFUNCTION(Server, Reliable)
	void ServerSetNicknameOnce(const FString& InNickname);
	
	UFUNCTION(Server, Reliable)
	void ServerSetReady();

	UFUNCTION()
	void OnRep_IsReady();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerJoinedLobby(const FString& InName);

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsReady)
	bool bIsReady;

	UPROPERTY(Replicated)
	FString BaseNickname;
	
	UPROPERTY(Replicated)
	FString DisplayNickname;

protected:
	FString SanitizeNickname_Server(const FString& InRaw) const;
};

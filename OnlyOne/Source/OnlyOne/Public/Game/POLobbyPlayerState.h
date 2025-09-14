// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "POLobbyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOOnReadyChanged, bool, bNowReady);

/**
 * 
 */
UCLASS()
class ONLYONE_API APOLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APOLobbyPlayerState();
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void InitNicknameFromGameInstanceOnce();
	
	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	bool IsReady() const { return bIsReady; }

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetBaseNickname() const { return BaseNickname; }

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetDisplayNickname() const { return DisplayNickname; }
	
	UPROPERTY(BlueprintAssignable, Category="PO|Lobby")
	FPOOnReadyChanged OnReadyChanged;

	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void ToggleReady();

public:
	UFUNCTION(Server, Reliable)
	void ServerSetNicknameOnce(const FString& InNickname);
	
	UFUNCTION(Server, Reliable)
	void ServerSetReady();

public:
	UFUNCTION()
	void OnRep_IsReady();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsReady)
	bool bIsReady;

	//TODO: BaseNickname과 DisplayNickname을 FJoinServerData와 통합하도록 변경해주세요.
	UPROPERTY(Replicated)
	FString BaseNickname;
	
	UPROPERTY(Replicated)
	FString DisplayNickname;

	UPROPERTY(Replicated)
	FJoinServerData PlayerData;

protected:
	FString SanitizeNickname_Server(const FString& InRaw) const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

};

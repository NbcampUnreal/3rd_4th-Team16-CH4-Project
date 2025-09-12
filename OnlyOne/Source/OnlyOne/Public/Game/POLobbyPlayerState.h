// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "POLobbyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOOnReadyChanged, bool, bNowReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOOnNicknameReady, const FString&, DisplayNickname);

/**
 * 
 */
UCLASS()
class ONLYONE_API APOLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APOLobbyPlayerState();
	
	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void InitNicknameFromGameInstanceOnce();
	
	UFUNCTION(BlueprintCallable, Category="PO|Lobby")
	void ToggleReady();
	
	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	bool IsReady() const { return bIsReady; }

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetBaseNickname() const { return BaseNickname; }

	UFUNCTION(BlueprintPure, Category="PO|Lobby")
	const FString& GetDisplayNickname() const { return DisplayNickname; }
	
	UPROPERTY(BlueprintAssignable, Category="PO|Lobby")
	FPOOnReadyChanged OnReadyChanged;

	UPROPERTY(BlueprintAssignable, Category="PO|Lobby")
	FPOOnNicknameReady OnNicknameReady;

public:
	UFUNCTION(Server, Reliable)
	void ServerSetNicknameOnce(const FString& InNickname);
	
	UFUNCTION(Server, Reliable)
	void ServerSetReady(bool bInReady);

public:
	UFUNCTION()
	void OnRep_IsReady();

	UFUNCTION()
	void OnRep_DisplayNickname();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsReady)
	bool bIsReady;
	
	UPROPERTY(Replicated)
	FString BaseNickname;
	
	UPROPERTY(ReplicatedUsing=OnRep_DisplayNickname)
	FString DisplayNickname;

protected:
	FString SanitizeNickname_Server(const FString& InRaw) const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

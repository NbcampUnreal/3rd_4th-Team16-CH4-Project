// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "POGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class ONLYONE_API UPOGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="PO|Profile")
	void SetPendingProfile(const FString& InNickname, const FString& InServerAddress)
	{
		PendingNickname = InNickname;
		PendingServerAddress = InServerAddress;
	}
	
	UFUNCTION(BlueprintPure, Category="PO|Profile")
	const FString& GetPendingNickname() const { return PendingNickname; }

	UFUNCTION(BlueprintPure, Category="PO|Profile")
	const FString& GetPendingServerAddress() const { return PendingServerAddress; }

private:
	UPROPERTY()
	FString PendingNickname;

	UPROPERTY()
	FString PendingServerAddress;
};

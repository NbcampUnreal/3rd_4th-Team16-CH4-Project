// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "POGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class ONLYONE_API UPOGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	//TODO: 닉네임과 서버주소를 FJoinServerData와 통합하도록 변경해주세요.
	UFUNCTION(Category="PO|Profile")
	void SetPendingProfile(FJoinServerData& InitUserData)
	{
		JoinServerData = InitUserData;
	}
	
	UFUNCTION(Category="PO|Profile")
	const FJoinServerData& GetPendingProfile() const
	{
		return JoinServerData;
	}

private:
	UPROPERTY()
	FJoinServerData JoinServerData;

};

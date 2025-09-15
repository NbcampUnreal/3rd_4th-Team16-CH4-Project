// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/POGameMode.h"

APlayerController* APOGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	
	FGenericTeamId TeamID = GetTeamIdForPlayer(NewPlayerController);
	                   
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamID);
	}
	
	NewPlayerController->StartSpot = FindPlayerStart(NewPlayerController);
	return NewPlayerController;
}

FGenericTeamId APOGameMode::GetTeamIdForPlayer(APlayerController* PlayerController)
{
	return FGenericTeamId(PlayerCount++);
}

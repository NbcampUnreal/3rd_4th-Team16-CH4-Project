// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/POMainMenuGameMode.h"

#include "Controllers/POMainMenuPlayerController.h"

APOMainMenuGameMode::APOMainMenuGameMode()
{
	PlayerControllerClass = APOMainMenuPlayerController::StaticClass();
}

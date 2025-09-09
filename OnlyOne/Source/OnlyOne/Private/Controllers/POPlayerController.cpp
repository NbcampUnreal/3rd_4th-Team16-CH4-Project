// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/POPlayerController.h"
#include "Components/UI/PlayerUIComponent.h"

APOPlayerController::APOPlayerController()
{
	PlayerUIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("Player UI Component"));
}

UPawnUIComponent* APOPlayerController::GetPawnUIComponent() const
{
	return PlayerUIComponent;
}

UPlayerUIComponent* APOPlayerController::GetPlayerUIComponent() const
{
	return PlayerUIComponent;
}

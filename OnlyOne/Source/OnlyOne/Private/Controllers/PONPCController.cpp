// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PONPCController.h"
#include "Components/UI/NpcUIComponent.h"

APONPCController::APONPCController()
{
	NpcUIComponent = CreateDefaultSubobject<UNpcUIComponent>("Npc UI Component");
}

UPawnUIComponent* APONPCController::GetPawnUIComponent() const
{
	return NpcUIComponent;
}

UNpcUIComponent* APONPCController::GetNpcUIComponent() const
{
	return NpcUIComponent;
}

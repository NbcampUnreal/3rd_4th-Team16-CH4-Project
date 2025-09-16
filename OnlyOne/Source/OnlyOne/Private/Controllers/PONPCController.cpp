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

ETeamAttitude::Type APONPCController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (GetPawn() == &Other)
	{
		return ETeamAttitude::Friendly;
	}
	
	return ETeamAttitude::Hostile;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/POCharacterBase.h"

APOCharacterBase::APOCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void APOCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APOCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APOCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POExitGameWidget.h"

#include "Controllers/Components/POUIStackingComponent.h"
#include "Controllers/Interfaces/POUIStackingInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UPOExitGameWidget::OnYesButtonClicked()
{
	// 게임 종료
	if (UWorld* World = GetWorld())
	{
		UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Quit, false);
	}
}
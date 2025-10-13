// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POReturnLobbyWidget.h"
#include "Kismet/GameplayStatics.h"

void UPOReturnLobbyWidget::OnYesButtonClicked()
{
	if (UWorld* World = GetWorld())
	{
		// 서버 연결을 끊고 메인 화면 레벨로 이동
		UGameplayStatics::OpenLevel(World, TEXT("L_MainMenu"));
	}
}

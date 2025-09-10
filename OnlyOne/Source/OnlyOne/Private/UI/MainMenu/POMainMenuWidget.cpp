// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/POMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

void UPOMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼 이벤트 바인딩
	if (JoinServerButton)
	{
		JoinServerButton->OnClicked.AddDynamic(this, &UPOMainMenuWidget::OnJoinServerClicked);
	}
	
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UPOMainMenuWidget::OnSettingsClicked);
	}
	
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPOMainMenuWidget::OnQuitClicked);
	}
}

void UPOMainMenuWidget::OnJoinServerClicked()
{
	// TODO: 서버 접속 로직 구현
	UE_LOG(LogTemp, Warning, TEXT("Join Server Clicked"));
}

void UPOMainMenuWidget::OnSettingsClicked()
{
	// TODO: 설정 화면 열기 로직 구현
	UE_LOG(LogTemp, Warning, TEXT("Settings Clicked"));
}

void UPOMainMenuWidget::OnQuitClicked()
{
	// 게임 종료
	if (UWorld* World = GetWorld())
	{
		UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Quit, false);
	}
}

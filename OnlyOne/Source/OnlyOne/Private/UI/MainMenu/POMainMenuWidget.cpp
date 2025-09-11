// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/POMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "UI/Common/POCustomButton.h"
#include "UI/MainMenu/POJoinServerWidget.h"

void UPOMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (JoinServerButton)
	{
		JoinServerButton->SetButtonText(FText::FromString("Join Server"));
		JoinServerButton->OnCustomButtonClicked.AddDynamic(this, &UPOMainMenuWidget::OnJoinServerClicked);
	}
	
	if (SettingsButton)
	{
		SettingsButton->SetButtonText(FText::FromString("Setting"));
		SettingsButton->OnCustomButtonClicked.AddDynamic(this, &UPOMainMenuWidget::OnSettingsClicked);
	}
	
	if (QuitButton)
	{
		QuitButton->SetButtonText(FText::FromString("Quit"));
		QuitButton->OnCustomButtonClicked.AddDynamic(this, &UPOMainMenuWidget::OnQuitClicked);
	}
}

void UPOMainMenuWidget::OnJoinServerClicked(UPOCustomButton* ClickedButton)
{
	// TODO: 서버 접속 로직 구현
	UE_LOG(LogTemp, Warning, TEXT("Join Server Clicked"));
	if (APOMainMenuPlayerController* PC = Cast<APOMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowJoinServer();
	}
}

void UPOMainMenuWidget::OnSettingsClicked(UPOCustomButton* ClickedButton)
{
	// TODO: 설정 화면 열기 로직 구현
	UE_LOG(LogTemp, Warning, TEXT("Settings Clicked"));
}

void UPOMainMenuWidget::OnQuitClicked(UPOCustomButton* ClickedButton)
{
	// 게임 종료
	if (UWorld* World = GetWorld())
	{
		UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Quit, false);
	}
}

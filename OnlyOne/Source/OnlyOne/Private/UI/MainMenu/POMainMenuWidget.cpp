// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/POMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "UI/Common/POCustomButton.h"
#include "UI/MainMenu/POJoinServerWidget.h"
#include "Kismet/GameplayStatics.h"

void UPOMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (HostServerButton)
	{
		HostServerButton->SetButtonText(FText::FromString("Host Server"));
		HostServerButton->OnCustomButtonClicked.AddDynamic(this, &UPOMainMenuWidget::OnHostServerClicked);
	}
	
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

	if (HowToPlayButton)
	{
		HowToPlayButton->SetButtonText(FText::FromString("How To Play"));
		HowToPlayButton->OnCustomButtonClicked.AddDynamic(this, &UPOMainMenuWidget::OnHowToPlayClicked);
	}
}

void UPOMainMenuWidget::NativeDestruct()
{
	if (HostServerButton)
	{
		HostServerButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOMainMenuWidget::OnHostServerClicked);
	}
	
	if (JoinServerButton)
	{
		JoinServerButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOMainMenuWidget::OnJoinServerClicked);
	}
	
	if (SettingsButton)
	{
		SettingsButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOMainMenuWidget::OnSettingsClicked);
	}
	
	if (QuitButton)
	{
		QuitButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOMainMenuWidget::OnQuitClicked);
	}

	if (HowToPlayButton)
	{
		HowToPlayButton->OnCustomButtonClicked.RemoveDynamic(this, &UPOMainMenuWidget::OnHowToPlayClicked);
	}
	
	Super::NativeDestruct();
}

void UPOMainMenuWidget::OnHostServerClicked(UPOCustomButton* ClickedButton)
{
	UE_LOG(LogTemp, Warning, TEXT("Host Server Clicked"));

	if (APOMainMenuPlayerController* PC = Cast<APOMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowHostServer();
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
	UE_LOG(LogTemp, Warning, TEXT("Settings Clicked"));
	if (APOMainMenuPlayerController* PC = Cast<APOMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowSettings();
	}
}

void UPOMainMenuWidget::OnQuitClicked(UPOCustomButton* ClickedButton)
{
	if (APOMainMenuPlayerController* PC = Cast<APOMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->OnExitGame();
	}
}

void UPOMainMenuWidget::OnHowToPlayClicked(UPOCustomButton* ClickedButton)
{
	if (APOMainMenuPlayerController* PC = Cast<APOMainMenuPlayerController>(GetOwningPlayer()))
	{
		PC->OnHowToPlay();
	}
}

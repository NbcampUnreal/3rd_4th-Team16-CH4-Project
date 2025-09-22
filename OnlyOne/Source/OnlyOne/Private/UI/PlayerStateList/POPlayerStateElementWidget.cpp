// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerStateList/POPlayerStateElementWidget.h"
#include "Components/TextBlock.h"

void UPOPlayerStateElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯들이 생성된 이후 캐시된 값 적용
	ApplyNickname();
	ApplyAliveState();
	ApplyKillCount();
}

void UPOPlayerStateElementWidget::SetPlayerNickname(const FString& InNickname)
{
	CachedNickname = InNickname;
	ApplyNickname();
}

void UPOPlayerStateElementWidget::SetIsAlive(bool bInAlive)
{
	bCachedAlive = bInAlive;
	ApplyAliveState();
}

void UPOPlayerStateElementWidget::SetKillCount(int32 InKillCount)
{
	CachedKillCount = InKillCount;
	ApplyKillCount();
}

void UPOPlayerStateElementWidget::ApplyNickname()
{
	if (NicknameText)
	{
		NicknameText->SetText(FText::FromString(CachedNickname));
	}
}

void UPOPlayerStateElementWidget::ApplyAliveState()
{
	if (AliveStateText)
	{
		const FText AliveText = FText::FromString(bCachedAlive ? TEXT(" ") : TEXT("DEAD"));
		AliveStateText->SetText(AliveText);
	}
}

void UPOPlayerStateElementWidget::ApplyKillCount()
{
	if (KillCountText)
	{
		KillCountText->SetText(FText::AsNumber(CachedKillCount));
	}
}

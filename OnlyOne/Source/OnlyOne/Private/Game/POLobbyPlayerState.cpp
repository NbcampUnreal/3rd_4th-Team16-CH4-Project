// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyPlayerState.h"

#include "Controllers/POServerLobbyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "game/POGameInstance.h"
#include "game/POBadWords.h"
#include "OnlyOne/OnlyOne.h"

APOLobbyPlayerState::APOLobbyPlayerState()
{
	bIsReady = false;
}

void APOLobbyPlayerState::BeginDestroy()
{
	OnReadyChanged.Clear();

	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwner()))
	{
		if (PC->OnPlayerReady.IsBound())
		{
			PC->OnPlayerReady.RemoveDynamic(this, &APOLobbyPlayerState::ToggleReady);
		}
	}
	
	Super::BeginDestroy();
}

void APOLobbyPlayerState::InitNicknameFromGameInstanceOnce()
{
	if (UPOGameInstance* GI = GetGameInstance<UPOGameInstance>())
	{
		const FString NickFromGI = GI->GetPendingNickname();
		ServerSetNicknameOnce(NickFromGI);
		LOG_NET(POLog, Warning, TEXT("Player %d nickname initialized from GI: %s"), GetPlayerId(), *BaseNickname);
	}

	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwner()))
	{
		PC->OnPlayerJoinLobby.Broadcast(PlayerData);
	}
}

void APOLobbyPlayerState::ToggleReady() //NOTE: OnToggleReady() 같은 이름이 더 좋아보임
{
	ServerSetReady();
}

void APOLobbyPlayerState::ServerSetNicknameOnce_Implementation(const FString& InNickname)
{
	if (!DisplayNickname.IsEmpty())
	{
		return;
	}
	
	FString Base = SanitizeNickname_Server(InNickname);
	if (Base.IsEmpty())
	{
		Base = TEXT("Player");
	}

	const int32 Tag = GetPlayerId() % 10000;
	const FString TagStr = FString::Printf(TEXT("#%04d"), Tag);

	//NOTE: 이 부분도 FJoinServerData에 통합되면 변경되야 합니다.
	BaseNickname    = Base;
	DisplayNickname = FString::Printf(TEXT("%s%s"), *Base, *TagStr);
	LOG_NET(POLog, Warning, TEXT("Player %d set nickname: %s"), GetPlayerId(), *DisplayNickname);
}

void APOLobbyPlayerState::ServerSetReady_Implementation() //NOTE: 매개변수 제거, Toggle에서는 매개변수가 필요 없음
{
	bIsReady = !bIsReady;
	OnRep_IsReady();
}

void APOLobbyPlayerState::OnRep_IsReady()
{
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwner()))
	{
		//TODO: FJoinServerData에 통합되면 이 부분도 변경되야 합니다.
		PlayerData.Name = BaseNickname;
		PC->OnReadyStateChanged.Broadcast(PlayerData, bIsReady);
	}
}

void APOLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APOLobbyPlayerState, bIsReady);
	DOREPLIFETIME(APOLobbyPlayerState, BaseNickname);
	DOREPLIFETIME(APOLobbyPlayerState, DisplayNickname);
}

void APOLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(GetOwner()))
	{
		PC->OnPlayerReady.AddDynamic(this, &APOLobbyPlayerState::ToggleReady);
	}

	if (GetNetMode() == NM_Client)
	{
		InitNicknameFromGameInstanceOnce();
	}

	
}

//TODO: Server RPC가 아닌데 _Server가 붙어있음. 이름 변경 필요
FString APOLobbyPlayerState::SanitizeNickname_Server(const FString& InRaw) const 
{
	FString S = InRaw;
	
	S = S.TrimStartAndEnd();
	
	{
		FString Compacted;
		Compacted.Reserve(S.Len());
		bool bPrevSpace = false;
		for (const TCHAR C : S)
		{
			const bool bSpace = FChar::IsWhitespace(C);
			if (bSpace)
			{
				if (!bPrevSpace)
				{
					Compacted.AppendChar(TEXT(' '));
					bPrevSpace = true;
				}
			}
			else
			{
				Compacted.AppendChar(C);
				bPrevSpace = false;
			}
		}
		S = Compacted.TrimStartAndEnd();
	}
	
	{
		FString Filtered;
		Filtered.Reserve(S.Len());

		for (TCHAR C : S)
		{
			const bool bAllowed =
				FChar::IsAlnum(C) ||
				(C >= 0xAC00 && C <= 0xD7A3) ||
				C == TEXT(' ') || C == TEXT('_') || C == TEXT('-');

			if (bAllowed)
			{
				Filtered.AppendChar(C);
			}
		}

		S = Filtered.TrimStartAndEnd();
		
		FString Compacted;
		Compacted.Reserve(S.Len());
		bool bPrevSpace = false;
		for (const TCHAR C : S)
		{
			const bool bSpace = (C == TEXT(' '));
			if (bSpace)
			{
				if (!bPrevSpace)
				{
					Compacted.AppendChar(TEXT(' '));
					bPrevSpace = true;
				}
			}
			else
			{
				Compacted.AppendChar(C);
				bPrevSpace = false;
			}
		}
		S = Compacted.TrimStartAndEnd();
	}
	
	constexpr int32 MinLen = 2;
	constexpr int32 MaxLen = 8;
	if (S.Len() > MaxLen)
	{
		S.LeftInline(MaxLen, EAllowShrinking::No);
	}
	if (S.Len() < MinLen)
	{
		S.Reset();
	}
	
	if (!S.IsEmpty() && POBadWords::ContainsProfanity(S))
	{
		S.Reset();
	}

	return S;
}

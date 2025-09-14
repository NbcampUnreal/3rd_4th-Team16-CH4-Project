// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyPlayerState.h"

#include "Controllers/POServerLobbyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "game/POGameInstance.h"
#include "game/POBadWords.h"
#include "Kismet/GameplayStatics.h"
#include "OnlyOne/OnlyOne.h"

APOLobbyPlayerState::APOLobbyPlayerState()
{
	bIsReady = false;
}

void APOLobbyPlayerState::BeginDestroy()
{
	OnReadyChanged.Clear();
	
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
	else
	{
		UE_LOG(POLog, Error, TEXT("Player %d InitNicknameFromGameInstanceOnce: GameInstance is null!"), GetPlayerId());
	}
}

void APOLobbyPlayerState::ToggleReady()
{
	ServerSetReady();
}

void APOLobbyPlayerState::ServerSetNicknameOnce_Implementation(const FString& InNickname)
{
	/*
	UE_LOG(POLog, Warning, TEXT("Player %d ServerSetNicknameOnce_Implementation: InNickname = '%s'"), GetPlayerId(), *InNickname);
	
	if (!DisplayNickname.IsEmpty())
	{
		UE_LOG(POLog, Warning, TEXT("Player %d ServerSetNicknameOnce_Implementation: DisplayNickname already set, returning"), GetPlayerId());
		return;
	}
	
	FString Base = SanitizeNickname_Server(InNickname);
	UE_LOG(POLog, Warning, TEXT("Player %d ServerSetNicknameOnce_Implementation: After sanitize Base = '%s'"), GetPlayerId(), *Base);
	
	if (Base.IsEmpty())
	{
		Base = TEXT("Player");
		UE_LOG(POLog, Warning, TEXT("Player %d ServerSetNicknameOnce_Implementation: Base was empty, using default 'Player'"), GetPlayerId());
	}
	*/

	const int32 Tag = GetPlayerId() % 10000;
	const FString TagStr = FString::Printf(TEXT("#%04d"), Tag);

	BaseNickname    = InNickname;
	DisplayNickname = InNickname;
	LOG_NET(POLog, Warning, TEXT("Player %d set nickname: %s"), GetPlayerId(), *DisplayNickname);

	MulticastPlayerJoinedLobby(BaseNickname);
}

void APOLobbyPlayerState::ServerSetReady_Implementation() //NOTE: 매개변수 제거, Toggle에서는 매개변수가 필요 없음
{
	bIsReady = !bIsReady;
	OnRep_IsReady();
}

void APOLobbyPlayerState::OnRep_IsReady()
{
	LOG_NET(POLog, Log, TEXT("Player %d OnRep_IsReady: bIsReady = %s"), GetPlayerId(), bIsReady ? TEXT("true") : TEXT("false"));
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		//TODO: FJoinServerData에 통합되면 이 부분도 변경되야 합니다.
		FJoinServerData PlayerData;
		PlayerData.Name = BaseNickname;
		PlayerData.DisplayNickname = DisplayNickname;
		PC->OnReadyStateChanged.Broadcast(PlayerData, bIsReady);
	}
}

void APOLobbyPlayerState::MulticastPlayerJoinedLobby_Implementation(const FString& InName)
{
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		FJoinServerData PlayerData;
		PlayerData.Name = InName;
		PlayerData.DisplayNickname = InName;
		PC->OnPlayerJoinLobby.Broadcast(PlayerData);
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

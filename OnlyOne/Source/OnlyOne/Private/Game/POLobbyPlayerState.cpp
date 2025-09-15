// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyPlayerState.h"

#include "Controllers/POServerLobbyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "game/POGameInstance.h"
#include "game/POBadWords.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "OnlyOne/OnlyOne.h"

APOLobbyPlayerState::APOLobbyPlayerState()
{
	bIsReady = false;
}

void APOLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	/*
	 *	NOTE: 지연호출을 하는 이유는 네트워크 동기화가 완료된 후에 실행되도록 하기 위함입니다.
	 *		하지만 지연호출을 하는 것이 완벽한 해결책은 아니며, 더 나은 방법이 있을 수 있습니다.
	 *		예를 들어, PlayerState가 초기화 됐을 때, 플래그를 설정하고, 그 플래그를 체크하는 방법 등이 있습니다.
	 */
	if (GetNetMode() == NM_Client)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, 
			[this]()
			{
				InitializeExistingPlayers();
				InitNicknameFromGameInstanceOnce();
			}, 
			1.0f, false);
	}

	
}

void APOLobbyPlayerState::BeginDestroy()
{
	Super::BeginDestroy();
}

void APOLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APOLobbyPlayerState, bIsReady);
	DOREPLIFETIME(APOLobbyPlayerState, BaseNickname);
	DOREPLIFETIME(APOLobbyPlayerState, DisplayNickname);
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

void APOLobbyPlayerState::MulticastPlayerLeftLobby_Implementation(const FString& InName)
{
	FJoinServerData PlayerData;
	PlayerData.Name = BaseNickname;
	PlayerData.DisplayNickname = DisplayNickname;
	if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		UE_LOG(POLog, Warning, TEXT("Broadcasting OnPlayerLeaveLobby for %s"), *BaseNickname);
		PC->OnPlayerLeaveLobby.Broadcast(PlayerData);
	}
}

void APOLobbyPlayerState::ServerSetNicknameOnce_Implementation(const FString& InNickname)
{
	/*
	 * NOTE: 아직 검증되지 않은 코드나, 현재 단계에서 계획되지 않은 기능이 포함되면
	 *			테스트 시 혼란을 야기할 수 있습니다. 필요시 주석을 해제하고 사용하세요.
	 */
	
	
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
	if (GetNetMode() == NM_ListenServer)
	{
		OnRep_IsReady();
	}
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

void APOLobbyPlayerState::InitializeExistingPlayers()
{
	if (GetNetMode() != NM_Client)
	{
		return;
	}

	/*
	 *	NOTE: 아래 소스코드는, 이벤트 구조나, 순회 효율 등 여러가지 측면에서 개선의 여지가 있습니다.
	 */
	if (AGameStateBase* GameState = GetWorld()->GetGameState())
	{
		if (APOServerLobbyPlayerController* PC = Cast<APOServerLobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			for (APlayerState* PS : GameState->PlayerArray)
			{
				if (APOLobbyPlayerState* LobbyPlayerState = Cast<APOLobbyPlayerState>(PS))
				{
					// 자기 자신은 제외하고, 닉네임이 설정된 플레이어만 추가
					if (LobbyPlayerState != this && !LobbyPlayerState->GetBaseNickname().IsEmpty())
					{
						FJoinServerData PlayerData;
						PlayerData.Name = LobbyPlayerState->GetBaseNickname();
						PlayerData.DisplayNickname = LobbyPlayerState->GetDisplayNickname();
						
						PC->OnPlayerJoinLobby.Broadcast(PlayerData);
						if (LobbyPlayerState->IsReady())
						{
							PC->OnReadyStateChanged.Broadcast(PlayerData, true);
						}
						
						UE_LOG(POLog, Log, TEXT("InitializeExistingPlayers: Added existing player %s (Ready: %s)"), 
							*LobbyPlayerState->GetBaseNickname(), 
							LobbyPlayerState->IsReady() ? TEXT("true") : TEXT("false"));
					}
				}
			}
		}
	}
}

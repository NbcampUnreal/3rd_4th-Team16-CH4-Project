// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POLobbyPlayerState.h"

#include "Controllers/POServerLobbyPlayerController.h"
#include "Controllers/POMainMenuPlayerController.h"
#include "Controllers/POPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "game/POGameInstance.h"
#include "game/POBadWords.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "OnlyOne/OnlyOne.h"
#include "game/POLobbyGameMode.h"

APOLobbyPlayerState::APOLobbyPlayerState()
{
	bIsReady = false;
	KillScore = 0;
	bIsAlive = true;
}

void APOLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	/*
	 *	NOTE: 지연호출을 하는 이유는 네트워크 동기화가 완료된 후에 실행되도록 하기 위함입니다.
	 *		하지만 지연호출을 하는 것이 완벽한 해결책은 아니며, 더 나은 방법이 있을 수 있습니다.
	 *		예를 들어, PlayerState가 초기화 됐을 때, 플래그를 설정하고, 그 플래그를 체크하는 방법 등이 있습니다.
	 */
	APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
	const bool bIsLocalClient =
		(GetNetMode() == NM_Client) ||
		(GetNetMode() == NM_ListenServer && OwnerPC && OwnerPC->IsLocalController());

	if (bIsLocalClient)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[this]()
			{
				InitializeExistingPlayers();
				InitNicknameFromGameInstanceOnce();
			},
			1.0f,
			false
		);
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
	
	DOREPLIFETIME(APOLobbyPlayerState, KillScore);
	DOREPLIFETIME(APOLobbyPlayerState, bIsAlive);
}

void APOLobbyPlayerState::InitNicknameFromGameInstanceOnce()
{
	if (UPOGameInstance* GI = GetGameInstance<UPOGameInstance>())
	{
		const FJoinServerData& Data = GI->GetPendingProfile();
		ServerSetNicknameOnce(Data.Name);

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
	
	const FString Base = InNickname.IsEmpty() ? TEXT("Player") : InNickname;
	int32 TagNum = 0;
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (UNetConnection* Conn = PC->NetConnection)
		{
			FString IdStr = Conn->PlayerId.IsValid() ? Conn->PlayerId->ToString() : TEXT("Invalid");
			TagNum = static_cast<int32>(FCrc::StrCrc32(*IdStr) % 10000);
		}
	}
	
	FString NumStr = FString::FromInt(FMath::Abs(TagNum) % 10000);
	const int32 Digits = 4;
	if (NumStr.Len() < Digits)
	{
		NumStr = FString::ChrN(Digits - NumStr.Len(), TEXT('0')) + NumStr;
	}
	const FString TagStr = TEXT("#") + NumStr;

	DisplayNickname = FString::Printf(TEXT("%s%s"), *Base, *TagStr);

	BaseNickname = DisplayNickname;
	
	UE_LOG(POLog, Warning, TEXT("Set nickname: %s (Tag=%s, PSId=%d)"),
		*DisplayNickname, *TagStr, GetPlayerId());

	MulticastPlayerJoinedLobby(BaseNickname);
	OnRep_DisplayNickname();
}

void APOLobbyPlayerState::ServerSetReady_Implementation()
{
	bIsReady = !bIsReady;

	if (HasAuthority())
	{
		if (APOLobbyGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<APOLobbyGameMode>() : nullptr)
		{
			GM->NotifyReadyStateChanged(this, bIsReady);
		}
	}

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

void APOLobbyPlayerState::PushSnapshotToLocalUI() const
{
	if (DisplayNickname.IsEmpty())
	{
		return;
	}

	LOG_NET(POLog, Warning, TEXT("[PS] PushSnapshotToLocalUI -> Nick=%s, Alive=%s, Kill=%d, Id=%d"),
	*DisplayNickname,
	bIsAlive ? TEXT("true") : TEXT("false"),
	KillScore,
	GetPlayerId());

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* LocalPCBase = UGameplayStatics::GetPlayerController(World, 0))
		{
			if (APOPlayerController* LocalPC = Cast<APOPlayerController>(LocalPCBase))
			{
				LocalPC->OnSetPlayerStateEntry.Broadcast(DisplayNickname, bIsAlive, KillScore);
				LOG_NET(POLog, Warning, TEXT("[PS] Broadcasted OnSetPlayerStateEntry for %s"), *DisplayNickname);
			}
		}
	}
}

void APOLobbyPlayerState::ServerResetForMatchStart()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsReady = false;
	KillScore = 0;
	bIsAlive = true;
}

void APOLobbyPlayerState::ServerAddKill_Implementation(int32 Delta)
{
	if (!HasAuthority())
	{
		return;
	}

	KillScore = FMath::Max(0, KillScore + Delta);
	OnRep_KillScore();
}

void APOLobbyPlayerState::ServerSetAlive_Implementation(bool bInAlive)
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAlive = bInAlive;
	OnRep_IsAlive();
}
void APOLobbyPlayerState::OnRep_DisplayNickname()
{
	LOG_NET(POLog, Warning, TEXT("Player %d OnRep_DisplayNickname -> %s"),
	GetPlayerId(), *DisplayNickname);
	
	PushSnapshotToLocalUI();
}

void APOLobbyPlayerState::AddKill_ServerOnly(int32 Delta)
{
	if (!HasAuthority())
	{
		return;
	}

	//TODO: 킬 이벤트 연결 후 테스트 예정
	KillScore = FMath::Max(0, KillScore + Delta);
	OnRep_KillScore();
}

void APOLobbyPlayerState::SetAlive_ServerOnly(bool bInAlive)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bIsAlive == bInAlive)
	{
		return;
	}

	bIsAlive = bInAlive;
	OnRep_IsAlive();
}

void APOLobbyPlayerState::OnRep_KillScore()
{
	LOG_NET(POLog, Warning, TEXT("Player %d OnRep_KillScore -> %d"),
	GetPlayerId(), KillScore);
	
	PushSnapshotToLocalUI();
}

void APOLobbyPlayerState::OnRep_IsAlive()
{
	LOG_NET(POLog, Warning, TEXT("Player %d OnRep_IsAlive -> %s"),
	GetPlayerId(), bIsAlive ? TEXT("true") : TEXT("false"));
	
	PushSnapshotToLocalUI();
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

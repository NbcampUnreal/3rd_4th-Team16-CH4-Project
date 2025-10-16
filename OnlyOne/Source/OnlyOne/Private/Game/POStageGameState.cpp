// Copyright Epic Games, Inc. All Rights Reserved.

#include "game/POStageGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "game/POLobbyPlayerState.h"
#include "OnlyOne/OnlyOne.h"

// enum → string (테스트 보조)
namespace
{
	static FString PhaseToStr(EPOStagePhase P)
	{
		if (const UEnum* E = StaticEnum<EPOStagePhase>())
		{
			return E->GetNameStringByValue(static_cast<int64>(P));
		}
		return FString::FromInt(static_cast<int32>(P));
	}
}

APOStageGameState::APOStageGameState()
{
	Phase = EPOStagePhase::Prep;
	PrepRemainingSeconds = 0;
	KillEventSerial = 0;

	LOG_NET(
		POLog,
		Log,
		TEXT("[StageGS] Ctor: Phase=%s, PrepRemaining=%d"),
		*PhaseToStr(Phase),
		PrepRemainingSeconds
	);
}

void APOStageGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		OnRep_Phase();
		OnRep_PrepRemainingSeconds();
		OnRep_StageRemainingSeconds();
		OnRep_AlivePlayerCount();
	}
}

void APOStageGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APOStageGameState, Phase);
	DOREPLIFETIME(APOStageGameState, PrepRemainingSeconds);
	DOREPLIFETIME(APOStageGameState, StageRemainingSeconds);
	DOREPLIFETIME(APOStageGameState, WinnerPS);
	DOREPLIFETIME(APOStageGameState, LastKillEvent);
	DOREPLIFETIME(APOStageGameState, AlivePlayerCount);
}

void APOStageGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PrepTimerHandle);
		World->GetTimerManager().ClearTimer(StageTimerHandle);
	}

	LOG_NET(POLog, Log, TEXT("[StageGS] EndPlay (Reason=%d)"), (int32)EndPlayReason);

	Super::EndPlay(EndPlayReason);
}

void APOStageGameState::ServerSetPhase_Implementation(EPOStagePhase NewPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	Phase = NewPhase;
	OnRep_Phase();
}

void APOStageGameState::ServerSetWinner_Implementation(APlayerState* InWinner)
{
	if (!HasAuthority())
	{
		return;
	}
	WinnerPS = InWinner;
	OnRep_WinnerPS();
}

void APOStageGameState::ServerClearWinner_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	WinnerPS = nullptr;
	OnRep_WinnerPS();
}

void APOStageGameState::ServerStartPrepCountdown(int32 InSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	LOG_NET(POLog, Warning, TEXT("[StageGS] ServerStartPrepCountdown(%d) called"), InSeconds);

	Phase = EPOStagePhase::Prep;
	OnRep_Phase();

	PrepRemainingSeconds = FMath::Max(0, InSeconds);
	OnRep_PrepRemainingSeconds();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PrepTimerHandle);

		if (PrepRemainingSeconds > 0)
		{
			LOG_NET(POLog, Warning, TEXT("[StageGS] Prep timer START: %d sec"), PrepRemainingSeconds);

			World->GetTimerManager().SetTimer(
				PrepTimerHandle,
				this,
				&APOStageGameState::TickPrepCountdown,
				1.0f,
				true
			);
		}
		else
		{
			LOG_NET(POLog, Warning, TEXT("[StageGS] No prep time -> switch to Active immediately"));

			Phase = EPOStagePhase::Active;
			OnRep_Phase();
		}
	}
}

void APOStageGameState::TickPrepCountdown()
{
	if (!HasAuthority())
	{
		return;
	}

	PrepRemainingSeconds = FMath::Max(0, PrepRemainingSeconds - 1);

	LOG_NET(POLog, Log, TEXT("[StageGS] Prep ticking... %d"), PrepRemainingSeconds);

	OnRep_PrepRemainingSeconds();

	if (PrepRemainingSeconds <= 0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(PrepTimerHandle);
		}

		LOG_NET(POLog, Warning, TEXT("[StageGS] Prep finished -> switch to Active"));

		Phase = EPOStagePhase::Active;
		OnRep_Phase();
	}
}

void APOStageGameState::ServerStartStageCountdown(int32 InSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	StageRemainingSeconds = FMath::Max(0, InSeconds);
	OnRep_StageRemainingSeconds();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StageTimerHandle);

		if (StageRemainingSeconds > 0)
		{
			LOG_NET(POLog, Warning, TEXT("[StageGS] Stage timer START: %d sec"), StageRemainingSeconds);

			World->GetTimerManager().SetTimer(
				StageTimerHandle,
				this,
				&APOStageGameState::TickStageCountdown,
				1.0f,
				true
			);
		}
		else
		{
			World->GetTimerManager().ClearTimer(StageTimerHandle);

			LOG_NET(POLog, Warning, TEXT("[StageGS] Stage timer expired immediately"));

			OnStageTimeExpired.Broadcast();
		}
	}
}

void APOStageGameState::TickStageCountdown()
{
	if (!HasAuthority())
	{
		return;
	}

	StageRemainingSeconds = FMath::Max(0, StageRemainingSeconds - 1);

	LOG_NET(POLog, Log, TEXT("[StageGS] Stage ticking... %d"), StageRemainingSeconds);

	OnRep_StageRemainingSeconds();

	if (StageRemainingSeconds <= 0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(StageTimerHandle);
		}

		LOG_NET(POLog, Warning, TEXT("[StageGS] Stage timer finished"));

		OnStageTimeExpired.Broadcast();
	}
}

void APOStageGameState::OnRep_Phase()
{
	LOG_NET(
		POLog,
		Warning,
		TEXT("[StageGS] OnRep_Phase -> %s (Auth=%s)"),
		*PhaseToStr(Phase),
		HasAuthority() ? TEXT("Server") : TEXT("Client")
	);

	OnPhaseChanged.Broadcast(Phase);
}

void APOStageGameState::OnRep_PrepRemainingSeconds()
{
	LOG_NET(
		POLog,
		Log,
		TEXT("[StageGS] OnRep_PrepRemainingSeconds -> %d (Auth=%s)"),
		PrepRemainingSeconds,
		HasAuthority() ? TEXT("Server") : TEXT("Client")
	);

	OnPrepTimeUpdated.Broadcast(PrepRemainingSeconds);
}

void APOStageGameState::OnRep_StageRemainingSeconds()
{
	OnStageTimeUpdated.Broadcast(StageRemainingSeconds);

	LOG_NET(
		POLog,
		Log,
		TEXT("[StageGS] OnRep_StageRemainingSeconds -> %d (Auth=%s)"),
		StageRemainingSeconds,
		HasAuthority() ? TEXT("Server") : TEXT("Client")
	);
}

void APOStageGameState::OnRep_WinnerPS()
{
	auto GetNicknameSafe = [](APlayerState* PS) -> FString
	{
		if (const APOLobbyPlayerState* LPS = Cast<APOLobbyPlayerState>(PS))
		{
			return LPS->GetDisplayNickname();
		}
		return PS ? PS->GetPlayerName() : TEXT("Unknown");
	};

	const FString WinnerName = GetNicknameSafe(WinnerPS);

	LOG_NET(POLog, Warning, TEXT("[StageGS] OnRep_WinnerPS: %s"), *WinnerName);

	OnWinnerDecided.Broadcast(WinnerPS);
}

void APOStageGameState::ServerPublishKillEvent(APlayerState* Killer, APlayerState* Victim)
{
	if (!HasAuthority())
	{
		return;
	}

	++KillEventSerial;
	LastKillEvent.Killer = Killer;
	LastKillEvent.Victim = Victim;
	LastKillEvent.Serial = KillEventSerial;

	LOG_NET(
		POLog,
		Log,
		TEXT("[StageGS] PublishKillEvent: Killer=%s, Victim=%s, Serial=%d"),
		Killer ? *Killer->GetPlayerName() : TEXT("None"),
		Victim ? *Victim->GetPlayerName() : TEXT("None"),
		LastKillEvent.Serial
	);

	OnRep_LastKillEvent();
}

void APOStageGameState::OnRep_LastKillEvent()
{
	auto GetNicknameSafe = [](APlayerState* PS) -> FString
	{
		if (const APOLobbyPlayerState* LPS = Cast<APOLobbyPlayerState>(PS))
		{
			return LPS->GetDisplayNickname();
		}
		return PS ? PS->GetPlayerName() : TEXT("Unknown");
	};

	const FString KillerName = GetNicknameSafe(LastKillEvent.Killer);
	const FString VictimName = GetNicknameSafe(LastKillEvent.Victim);

	OnKillEvent.Broadcast(KillerName, VictimName);

	LOG_NET(POLog, Log, TEXT("[StageGS] OnRep_LastKillEvent: %s ▶ %s (Serial=%d)"),
		*KillerName, *VictimName, LastKillEvent.Serial);
}

void APOStageGameState::ServerSetAlivePlayerCount_Implementation(int32 NewCount)
{
	if (!HasAuthority())
	{
		return;
	}

	if (NewCount < 0)
	{
		NewCount = 0;
	}

	if (AlivePlayerCount != NewCount)
	{
		AlivePlayerCount = NewCount;
		
		OnAliveCountChanged.Broadcast(AlivePlayerCount);
		UE_LOG(LogTemp, Verbose, TEXT("[StageGS] ServerSetAlivePlayerCount -> %d"), AlivePlayerCount);
	}
}

void APOStageGameState::OnRep_AlivePlayerCount()
{
	// 클라이언트에서 UI로 전달
	OnAliveCountChanged.Broadcast(AlivePlayerCount);
	OnAliveCountChangedBP.Broadcast(AlivePlayerCount);

	UE_LOG(LogTemp, Verbose, TEXT("[StageGS] OnRep_AlivePlayerCount -> %d"), AlivePlayerCount);
}

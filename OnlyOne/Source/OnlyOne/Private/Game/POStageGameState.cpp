// Copyright Epic Games, Inc. All Rights Reserved.
#include "game/POStageGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "OnlyOne/OnlyOne.h"

// 테스트 용 로그 (enum -> string)
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

	LOG_NET(POLog, Log, TEXT("[StageGS] Ctor: Phase=%s, PrepRemaining=%d"),
	*PhaseToStr(Phase), PrepRemainingSeconds);
}

void APOStageGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APOStageGameState, Phase);
	DOREPLIFETIME(APOStageGameState, PrepRemainingSeconds);
}

void APOStageGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PrepTimerHandle);
	}
	LOG_NET(POLog, Log, TEXT("[StageGS] EndPlay (Reason=%d)"), (int32)EndPlayReason);
	Super::EndPlay(EndPlayReason);
}

void APOStageGameState::ServerStartPrepCountdown(int32 InSeconds)
{
	if (!HasAuthority()) return;
	
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
			World->GetTimerManager().SetTimer(PrepTimerHandle, this, &APOStageGameState::TickPrepCountdown, 1.0f, true);
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
	if (!HasAuthority()) return;

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

void APOStageGameState::OnRep_Phase()
{
	LOG_NET(POLog, Warning, TEXT("[StageGS] OnRep_Phase -> %s (Auth=%s)"),
	*PhaseToStr(Phase), HasAuthority() ? TEXT("Server") : TEXT("Client"));
	OnPhaseChanged.Broadcast(Phase);
}

void APOStageGameState::OnRep_PrepRemainingSeconds()
{
	LOG_NET(POLog, Log, TEXT("[StageGS] OnRep_PrepRemainingSeconds -> %d (Auth=%s)"),
	PrepRemainingSeconds, HasAuthority() ? TEXT("Server") : TEXT("Client"));
	OnPrepTimeUpdated.Broadcast(PrepRemainingSeconds);
}
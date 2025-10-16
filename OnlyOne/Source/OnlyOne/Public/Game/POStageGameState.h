// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "POStageGameState.generated.h"

UENUM(BlueprintType)
enum class EPOStagePhase : uint8
{
	Prep	UMETA(DisplayName="Prep"),
	Active	UMETA(DisplayName="Active"),
	RoundEnd UMETA(DisplayName="RoundEnd"),
	GameEnd  UMETA(DisplayName="GameEnd")
};

/** ===== Kill Event Payload ===== */
USTRUCT(BlueprintType)
struct FPOKillEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	APlayerState* Killer = nullptr;

	UPROPERTY(BlueprintReadOnly)
	APlayerState* Victim = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 Serial = 0;

	FPOKillEvent() {}
	FPOKillEvent(APlayerState* InKiller, APlayerState* InVictim, int32 InSerial)
		: Killer(InKiller), Victim(InVictim), Serial(InSerial)
	{}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPhaseChanged, EPOStagePhase);
DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnPrepTimeUpdated, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnStageTimeUpdated, int32);
DECLARE_MULTICAST_DELEGATE(FPOOnStageTimeExpired);
DECLARE_MULTICAST_DELEGATE_OneParam(FPOOnWinnerDecided, APlayerState*);

/** C++용(비동적) 생존자 수 변경 델리게이트 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAliveCountChanged, int32 /*NewAliveCount*/);
/** BP용(동적) 생존자 수 변경 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAliveCountChangedBP, int32, NewAliveCount);

/**
 * Stage GameState
 */
UCLASS()
class ONLYONE_API APOStageGameState : public AGameState
{
	GENERATED_BODY()

public:
	APOStageGameState();

	/* ===== Phase ===== */
	UFUNCTION(BlueprintPure, Category="PO|Phase")
	EPOStagePhase GetPhase() const { return Phase; }

	UFUNCTION(Server, Reliable, Category="PO|Phase")
	void ServerSetPhase(EPOStagePhase NewPhase);

	UFUNCTION(BlueprintPure, Category="PO|Phase")
	int32 GetPrepRemainingSeconds() const { return PrepRemainingSeconds; }

	UFUNCTION(Category="PO|Phase")
	void ServerStartPrepCountdown(int32 InSeconds);

	/* ===== Stage Time ===== */
	UFUNCTION(BlueprintPure, Category="PO|StageTime")
	int32 GetStageRemainingSeconds() const { return StageRemainingSeconds; }

	UFUNCTION(Category="PO|StageTime")
	void ServerStartStageCountdown(int32 InSeconds);

	/* ===== Winner ===== */
	UFUNCTION(BlueprintPure, Category="PO|Result")
	APlayerState* GetWinnerPlayerState() const { return WinnerPS; }

	UFUNCTION(Server, Reliable, Category="PO|Result")
	void ServerSetWinner(APlayerState* InWinner);

	UFUNCTION(Server, Reliable, Category="PO|Result")
	void ServerClearWinner();

	/* ===== Delegates ===== */
	FPOOnPhaseChanged OnPhaseChanged;
	FPOOnPrepTimeUpdated OnPrepTimeUpdated;
	FPOOnStageTimeUpdated OnStageTimeUpdated;
	FPOOnStageTimeExpired OnStageTimeExpired;
	FPOOnWinnerDecided OnWinnerDecided;

	/** (이전 정의 정리) 생존자 수 변경: BP에서 바인딩할 수 있도록 동적 델리게이트 제공 */
	UPROPERTY(BlueprintAssignable, Category="PO|Alive")
	FOnAliveCountChangedBP OnAliveCountChangedBP;

	/** C++에서 바인딩할 수 있는 비동적 델리게이트 */
	FOnAliveCountChanged OnAliveCountChanged;

	/** 서버 전용: 생존자 수 설정(RepNotify 트리거) */
	UFUNCTION(Server, Reliable, Category="PO|Alive")
	void ServerSetAlivePlayerCount(int32 NewCount);

	/** 현재 생존자 수(RepNotify) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AlivePlayerCount, Category="PO|Alive")
	int32 AlivePlayerCount = 0;

	/** ===== Kill Event: UI 전달용 델리게이트 및 API ===== */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPOOnKillEvent, const FString&, KillerName, const FString&, VictimName);

	/** UI가 구독할 델리게이트 (Killer/Victim 전달) */
	UPROPERTY(BlueprintAssignable, Category="PO|Kill")
	FPOOnKillEvent OnKillEvent;

	UFUNCTION(Category="PO|Event")
	void ServerPublishKillEvent(APlayerState* Killer, APlayerState* Victim);

protected:
	/* ===== Unreal Lifecycle & RepNotify ===== */
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnRep_Phase();

	UFUNCTION()
	void OnRep_PrepRemainingSeconds();

	UFUNCTION()
	void OnRep_StageRemainingSeconds();

	UFUNCTION()
	void OnRep_WinnerPS();

	/** 킬 이벤트 복제 알림 → 모든 클라에서 델리게이트 브로드캐스트 */
	UFUNCTION()
	void OnRep_LastKillEvent();

	/** AlivePlayerCount 복제 알림 → 클라에서 생존자 수 델리게이트 브로드캐스트 */
	UFUNCTION()
	void OnRep_AlivePlayerCount();

protected:
	/* ===== Replicated State ===== */
	UPROPERTY(ReplicatedUsing=OnRep_Phase)
	EPOStagePhase Phase;

	UPROPERTY(ReplicatedUsing=OnRep_PrepRemainingSeconds)
	int32 PrepRemainingSeconds;

	UPROPERTY(ReplicatedUsing=OnRep_StageRemainingSeconds)
	int32 StageRemainingSeconds = 0;

	UPROPERTY(ReplicatedUsing=OnRep_WinnerPS)
	APlayerState* WinnerPS = nullptr;

	/** 마지막 킬 이벤트 (구조체에 일련번호 포함) */
	UPROPERTY(ReplicatedUsing=OnRep_LastKillEvent)
	FPOKillEvent LastKillEvent;

private:
	/* ===== Timers & Internal Ticks ===== */
	FTimerHandle PrepTimerHandle;
	FTimerHandle StageTimerHandle;

	void TickPrepCountdown();
	void TickStageCountdown();

	int32 KillEventSerial = 0;
};
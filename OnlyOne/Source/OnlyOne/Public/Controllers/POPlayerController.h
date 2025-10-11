// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterControllerBase.h"
#include "Interfaces/POUIStackingInterface.h"
#include "POPlayerController.generated.h"

class UPOReturnLobbyWidget;
class UPOExitGameWidget;
enum class EPOStagePhase : uint8;
class UPOPrevTimerWidget;
class UPOSettingWidget;
class UPOInGameMenuWidget;
class UPOUIStackingComponent;
class UPOPlayerStateListWidget;
class UPODataAsset_InputConfig;
class APOPlayerCharacter;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FPOOnSetPlayerStateEntry, const FString& /*Nickname*/, bool /*bIsAlive*/, int32 /*KillCount*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPOOnTimerChanged, int32, NewTimeSeconds);

USTRUCT()
struct FPOPlayerStateEntry
{
	GENERATED_BODY()

	FString Nickname;
	bool bIsAlive = true;
	int32 KillCount = 0;
};

UCLASS()
class ONLYONE_API APOPlayerController : public APOCharacterControllerBase, public IPOUIStackingInterface
{
	GENERATED_BODY()

public:
	APOPlayerController();

protected:
#pragma region Lifecycle
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

public:
#pragma region Interfaces
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
#pragma endregion

#pragma region Spectator
	void StartSpectating(const APawn* DeadCharacter);
	void SpectatorNextTarget();
	void SpectatorPreviousTarget(); 
#pragma endregion
	
private:
#pragma region Core Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerUIComponent> PlayerUIComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPODataAsset_InputConfig> InputConfigDataAsset;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
  
#pragma endregion

#pragma region Spectator Internals
	UPROPERTY()
	TArray<TObjectPtr<APOPlayerCharacter>> SpectatorTargets;

	UPROPERTY()
	TObjectPtr<APawn> DiedPawn;

	UPROPERTY()
	TObjectPtr<APlayerState> DiedPlayerState;

	int32 CurrentSpectatorIndex = 0;

	void BuildSpectatorTargets();
	void CycleSpectator(int32 Direction); // 순환 로직을 처리할 공통 함수 추가
#pragma endregion

	/* UI 섹션 */
private:
	bool bIsListVisible = false;

public:
	// NOTE: 외부에서 Broadcast하여 리스트를 갱신할 수 있는 델리게이트
	FPOOnSetPlayerStateEntry OnSetPlayerStateEntry;

	// Timer 변경을 알리는 델리게이트 (BP에서 바인드 가능)
	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FPOOnTimerChanged OnPreTimerChanged;

	void OnChangeGamePhase(EPOStagePhase NewPhase);
	
	// 위젯 생성/표시/숨김
	void EnsureListWidgetCreated();
	void ShowListWidget();
	void HideListWidget();
	void ShowHUDWidget();
	void HideHUDWidget();
	void ToggleListWidget();
	void ShowInGameMenuWidget();
	void ShowSettingWidget();
	void ShowQuitGameWidget();
	void ShowRetunToLobbyWidget();
	void ShowPrevTimerWidget();
	void HidePrevTimerWidget();
	void ShowExitGameWidget();

	// 사망 시 표시되는 스펙테이터 도움말 위젯
	void ShowSpectatorHelpWidget();
	void HideSpectatorHelpWidget();

	void OnEscapeMenu();

	virtual UPOUIStackingComponent* GetUIStackingComponent() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TObjectPtr<UPOUIStackingComponent> UIStackingComponent;
	
	// Player State List Widget
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPOPlayerStateListWidget> PlayerStateListWidgetClass;
	UPROPERTY()
	TObjectPtr<UPOPlayerStateListWidget> PlayerStateListWidget;
	
	TQueue<FPOPlayerStateEntry> PlayerStateQueue;

	//HUD Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidgetInstance;

	// InGame Menu Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UPOInGameMenuWidget> InGameMenuWidgetClass;
	UPROPERTY()
	TObjectPtr<UPOInGameMenuWidget> InGameMenuWidgetInstance;

	// Setting Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UPOSettingWidget> SettingWidgetClass;
	UPROPERTY()
	TObjectPtr<UPOSettingWidget> SettingWidgetInstance;

	// Prev Timer Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UPOPrevTimerWidget> PrevTimerWidgetClass;
	UPROPERTY()
	TObjectPtr<UPOPrevTimerWidget> PrevTimerWidgetInstance;

	// Spectator Help Widget (WBP 할당 필요)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> SpectatorHelpWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> SpectatorHelpWidget;

	// 게임 종료 확인 위젯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOExitGameWidget> ExitGameWidgetClass; // 게임 종료 확인 위젯 클래스
	UPROPERTY()
	TObjectPtr<UPOExitGameWidget> ExitGameWidget; // 게임 종료 확인 위젯 인스턴스

	// 메인 메뉴로 돌아가기 확인 위젯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPOReturnLobbyWidget> ReturnToLobbyWidgetClass; // 메인 메뉴
	UPROPERTY()
	TObjectPtr<UPOReturnLobbyWidget> ReturnToLobbyWidgetInstance; // 메인 메뉴로 돌아

private:
	void OnPlayerStateUpdated(const FString& Nickname, bool bIsAlive, int32 KillCount);
};


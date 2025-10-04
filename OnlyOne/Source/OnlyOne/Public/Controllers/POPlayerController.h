// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterControllerBase.h"
#include "Interfaces/POUIStackingInterface.h"
#include "POPlayerController.generated.h"

class UPOSettingWidget;
class UPOInGameMenuWidget;
class UPOUIStackingComponent;
class UPOPlayerStateListWidget;
class UPODataAsset_InputConfig;
class APOPlayerCharacter;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FPOOnSetPlayerStateEntry, const FString& /*Nickname*/, bool /*bIsAlive*/, int32 /*KillCount*/);

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

private:
	void OnPlayerStateUpdated(const FString& Nickname, bool bIsAlive, int32 KillCount);
};
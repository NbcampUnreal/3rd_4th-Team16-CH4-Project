// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterControllerBase.h"
#include "POPlayerController.generated.h"

class UPODataAsset_InputConfig;
class APOPlayerCharacter;

UCLASS()
class ONLYONE_API APOPlayerController : public APOCharacterControllerBase
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
#pragma endregion
};
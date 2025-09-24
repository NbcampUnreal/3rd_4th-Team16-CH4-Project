// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POCharacterControllerBase.h"
#include "POPlayerController.generated.h"

class UPOPlayerStateListWidget;
/**
 * 
 */


DECLARE_MULTICAST_DELEGATE_ThreeParams(FPOOnSetPlayerStateEntry, const FString& /*Nickname*/, bool /*bIsAlive*/, int32 /*KillCount*/);

UCLASS()
class ONLYONE_API APOPlayerController : public APOCharacterControllerBase
{
	GENERATED_BODY()
	
public:
	APOPlayerController();
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerUIComponent> PlayerUIComponent;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	/* UI 섹션 */
public:
	// NOTE: 외부에서 Broadcast하여 리스트를 갱신할 수 있는 델리게이트
	FPOOnSetPlayerStateEntry OnSetPlayerStateEntry;
	
	// 위젯 생성/표시/숨김
	void EnsureListWidgetCreated();
	void ShowListWidget();
	void HideListWidget();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPOPlayerStateListWidget> PlayerStateListWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UPOPlayerStateListWidget> PlayerStateListWidget;
};

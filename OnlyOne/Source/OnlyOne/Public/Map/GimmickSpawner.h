#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GimmickSpawner.generated.h"

UCLASS()
class ONLYONE_API AGimmickSpawner : public AActor
{
	GENERATED_BODY()

public:
	AGimmickSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// NavMesh 준비되면 폴링 중지하고 스폰
	void TrySpawnWhenNavReady();
	void DoSpawn(); // 실제 스폰 루틴

	// ====== 설정 ======
	// 스폰 규칙이 들어있는 DataTable(FGimmickSpawnRow)
	UPROPERTY(EditAnywhere, Category="Config")
	UDataTable* Table = nullptr;

	// 탐색 중심. 미지정 시 스포너 자신의 위치가 기준
	UPROPERTY(EditAnywhere, Category="Config")
	AActor* Center = nullptr;

	// 결정적 랜덤
	UPROPERTY(EditAnywhere, Category="Random")
	bool bDeterministic = false;

	UPROPERTY(EditAnywhere, Category="Random", meta=(EditCondition="bDeterministic"))
	int32 Seed = 1234;

	// 내부
	FTimerHandle NavWaitTimer;
};

#include "Map/GimmickSpawner.h"
#include "NavigationSystem.h"
#include "Engine/DataTable.h"
#include "Map/GimmickSpawnRow.h"   // FGimmickSpawnRow 선언 헤더
#include "DrawDebugHelpers.h"

AGimmickSpawner::AGimmickSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AGimmickSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority() || !Table) return;

    // NavMesh가 준비될 때까지 0.25초 간격으로 확인
    GetWorldTimerManager().SetTimer(
        NavWaitTimer, this, &ThisClass::TrySpawnWhenNavReady, 0.25f, true, 0.f);
}

void AGimmickSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(NavWaitTimer);
    Super::EndPlay(EndPlayReason);
}

void AGimmickSpawner::TrySpawnWhenNavReady()
{
    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    const ANavigationData* NavData =
        NavSys ? NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate) : nullptr;

    const bool bReady = (NavData != nullptr) && !NavSys->IsNavigationBeingBuilt(GetWorld());
    if (!bReady) return;

    GetWorldTimerManager().ClearTimer(NavWaitTimer);
    DoSpawn();
}

void AGimmickSpawner::DoSpawn()
{
    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return;

    FRandomStream Rng = bDeterministic ? FRandomStream(Seed)
                                       : FRandomStream(FMath::Rand());

    // 테이블 로우 가져오기
    TArray<FGimmickSpawnRow*> Rows;
    static const FString Ctx = TEXT("GimmickSpawner");
    Table->GetAllRows<FGimmickSpawnRow>(Ctx, Rows);
    if (Rows.Num() == 0) return;

    const FVector Origin = Center ? Center->GetActorLocation() : GetActorLocation();

    for (const FGimmickSpawnRow* Row : Rows)
    {
        if (!Row || !Row->ActorClass || Row->Count <= 0) continue;

        for (int32 i = 0; i < Row->Count; ++i)
        {
            // NavMesh 위 임의 점 구하기
            FNavLocation Pick;
            const bool bOk = (Row->SearchRadius > 0.f)
                ? NavSys->GetRandomPointInNavigableRadius(Origin, Row->SearchRadius, Pick) // 반경 내
                : NavSys->GetRandomPoint(Pick);                                           // Nav 전체

            if (!bOk) continue;

            const FRotator Rot(0.f, Rng.FRandRange(0.f, 360.f), 0.f);

            FActorSpawnParameters Params;
            Params.Owner = this;
            Params.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            if (AActor* Spawned = GetWorld()->SpawnActor<AActor>(Row->ActorClass, Pick.Location, Rot, Params))
            {
                // 멀티플레이 표시 보장
                Spawned->SetReplicates(true);
                Spawned->SetReplicateMovement(true);
#if WITH_EDITOR
                DrawDebugSphere(GetWorld(), Pick.Location, 40.f, 12, FColor::Green, false, 8.f);
#endif
            }
        }
    }
}

#pragma once

#include "CoreMinimal.h"
#include "GimmickSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FGimmickSpawnRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> ActorClass = nullptr; // 스폰할 기믹

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
    int32 Count = 10;                           // 몇 개

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
    float SearchRadius = 5000.f;               // 중심에서 탐색 반경
};
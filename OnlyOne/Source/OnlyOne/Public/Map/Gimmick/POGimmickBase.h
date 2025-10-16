
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "POGimmickBase.generated.h"


class UBoxComponent;
class UAudioComponent;
class UPrimitiveComponent;
class UHierarchicalInstancedStaticMeshComponent;
class UInstancedStaticMeshComponent;
UCLASS()
class ONLYONE_API APOGimmickBase : public AActor
{
	GENERATED_BODY()
	
public:
	APOGimmickBase();

protected:
#pragma region Components
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxCollision;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

#pragma endregion 

#pragma region config
	
	UPROPERTY(EditAnywhere, Category="Net")
	bool bServerOnly = true;

	UPROPERTY(EditAnywhere,Category="Gimmick")
	bool bDestroyed = true;

#pragma endregion
	virtual void BeginPlay() override;
	
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* comp, AActor* other, UPrimitiveComponent* otherComp
		, int32 BoxIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(BlueprintNativeEvent)
	bool CanActivate(AActor* Target) const;
	UFUNCTION(BlueprintNativeEvent)
	void ActivateGimmick(AActor* Target);
	UFUNCTION(BlueprintNativeEvent)
	void OnGimmickComplete(AActor* Target);

	static class UAbilitySystemComponent* GetASC(AActor* Actor);


#pragma region Network
public:
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bConsumed =false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Network", meta=(ClampMin="0.0"))
	float NetCullDistance = 750.f;

#pragma endregion


};
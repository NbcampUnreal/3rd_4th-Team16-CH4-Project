// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "POUIStackingComonent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONLYONE_API UPOUIStackingComonent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPOUIStackingComonent();
	
	void PushWidget(UUserWidget* Widget);
	void PopWidget();
	void PopToWidget(UUserWidget* Widget);
	void ClearStack();
	
	FORCEINLINE UUserWidget* GetTopWidget() const { return UIStack.Num() > 0 ? UIStack.Last() : nullptr; }
	FORCEINLINE bool IsEmpty() const { return UIStack.IsEmpty(); }

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> UIStack;
};

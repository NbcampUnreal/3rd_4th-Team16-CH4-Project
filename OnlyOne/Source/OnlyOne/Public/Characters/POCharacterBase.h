// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/PawnCombatInterface.h"
#include "POCharacterBase.generated.h"

class UPOAttributeSet;
class UInputComponent;
class UPOAbilitySystemComponent;

UCLASS()
class ONLYONE_API APOCharacterBase :
	public ACharacter,
	public IAbilitySystemInterface,
    public IGenericTeamAgentInterface,
	public IPawnCombatInterface
{
	GENERATED_BODY()

public:
	APOCharacterBase();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

protected:
	virtual void PossessedBy(AController* NewController) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UPOAbilitySystemComponent* POAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UPOAttributeSet* POAttributeSet;

private:
	void ServerSideInit();
	void ClientSideInit();
	
	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamID;

	UFUNCTION()
	virtual void OnRep_TeamID();
	
public:
	FORCEINLINE UPOAbilitySystemComponent* GetPOAbilitySystemComponent() const { return POAbilitySystemComponent; }
	FORCEINLINE UPOAttributeSet* GetPOAttributeSet() const { return POAttributeSet; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "POAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPOAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UPOAttributeSet, CurrentHealth);
	ATTRIBUTE_ACCESSORS(UPOAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UPOAttributeSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UPOAttributeSet, DamageTaken);
	ATTRIBUTE_ACCESSORS(UPOAttributeSet, SmokeGrenadeCount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attribute | Player State", ReplicatedUsing = OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute | Player State", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute | Player Combat", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute | Player Combat", ReplicatedUsing = OnRep_DamageTaken)
	FGameplayAttributeData DamageTaken;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute | Items", ReplicatedUsing = OnRep_SmokeGrenadeCount)
	FGameplayAttributeData SmokeGrenadeCount;

	
	UFUNCTION()
	void OnRep_CurrentHealth(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DamageTaken(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_SmokeGrenadeCount(const FGameplayAttributeData& OldValue);
};

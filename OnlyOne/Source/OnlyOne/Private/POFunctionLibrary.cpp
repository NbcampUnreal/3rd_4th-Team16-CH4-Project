// Fill out your copyright notice in the Description page of Project Settings.


#include "POFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/PawnCombatInterface.h"

UPawnCombatComponent* UPOFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EPOValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);
	OutValidType = CombatComponent ? EPOValidType::EWT_Valid : EPOValidType::EWT_Invalid;
	return CombatComponent;
}

bool UPOFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn)

	IGenericTeamAgentInterface* GenericTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (GenericTeamAgent && TargetTeamAgent)
	{
		return GenericTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}

	// 팀 인터페이스를 구현하지않는경우 적대 관계로 간주
	return true;
}

UPawnCombatComponent* UPOFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (const IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}

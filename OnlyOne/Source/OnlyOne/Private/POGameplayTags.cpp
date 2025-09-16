// Fill out your copyright notice in the Description page of Project Settings.


#include "POGameplayTags.h"

namespace POGameplayTags
{
	/* 캐릭터 Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Walk, "InputTag.Walk")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint")

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack, "InputTag.Attack")

	/* Player Tags */
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack, "Player_Ability_Attack")

	/* Shard Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit")

	/* GameplayCue Tags*/
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_GimmickTag_Sound, "GameplayCue.Gimmick.Sound")
}

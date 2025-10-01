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
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Spectator_Next, "InputTag.Spectator.Next")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Spectator_Previous, "InputTag.Spectator.Previous")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SmokeGrenade, "InputTag.SmokeGrenade")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interaction, "InputTag.Interaction")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ListWidget, "InputTag.ListWidget");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ListWidget_Show, "InputTag.ListWidget.Show")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ListWidget_Hide, "InputTag.ListWidget.Hide")

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack, "InputTag.Attack")

	/* Player Tags */
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability, "Player.Ability")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack, "Player.Ability.Attack")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SmokeGrenade, "Player.Ability.SmokeGrenade")
	
	/* Player State */
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Death, "Shared.Status.Death")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Slow,"Shared.Status.Slow")

	/* Event Tags */
	UE_DEFINE_GAMEPLAY_TAG(Event_Death, "Event.Death")

	/* Shard Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit")

	/* GameplayCue Tags*/
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_GimmickTag_Sound, "GameplayCue.Gimmick.Sound")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Status_Slow,"GameplayCue.Status.Slow")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Smoke,"GameplayCue.Effect.Smoke")
}

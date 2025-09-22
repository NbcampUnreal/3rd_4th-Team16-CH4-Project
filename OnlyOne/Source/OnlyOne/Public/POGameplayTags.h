// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace POGameplayTags
{
	/* 캐릭터 Input Tags */
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Walk);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);

	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Attack);

	/* Player Tags */
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack);

	/* Player State */
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Death);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_Death);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Slow);

	/* Shard Tags*/
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_SetByCaller_BaseDamage);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_MeleeHit);
	
   /* GameplayCue Tags*/
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_GimmickTag_Sound);
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Status_Slow);

}

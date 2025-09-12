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

	/* Shard Tags*/
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_SetByCaller_BaseDamage);

	/* Event Tags*/
	ONLYONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Attack_Hit);
}

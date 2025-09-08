// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlyOne.h"
#include "Modules/ModuleManager.h"
#include "Core/POGameplayTags.h"

class FOnlyOneModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		// 부모 클래스의 StartupModule 호출
		FDefaultGameModuleImpl::StartupModule();
		
		// GameplayTags 초기화
		FPOGameplayTags::InitializeNativeTags();
	}
	
	virtual void ShutdownModule() override
	{
		// 부모 클래스의 ShutdownModule 호출
		FDefaultGameModuleImpl::ShutdownModule();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FOnlyOneModule, OnlyOne, "OnlyOne" );
DEFINE_LOG_CATEGORY(POLog);
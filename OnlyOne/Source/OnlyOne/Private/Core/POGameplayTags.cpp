// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/POGameplayTags.h"

#include "GameplayTagsManager.h"

// 정적 멤버 변수 정의
FGameplayTag FPOGameplayTags::Test_TestTag;
bool FPOGameplayTags::bIsInitialized = false;

void FPOGameplayTags::InitializeNativeTags()
{
	if (bIsInitialized)
	{
		return; // 중복 초기화 방지
	}

	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	//TODO: 헤더에 추가한 태그를 이곳에도 추가하세요.
	Test_TestTag = TagsManager.AddNativeGameplayTag(TEXT("Test.TestTag"), TEXT("A test tag for demonstration purposes."));
	
	bIsInitialized = true;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"

/**
 * @namespace Debug
 * @brief 개발 중 디버깅 메시지를 화면(On-Screen Debug Message)과 Output Log에 편리하게 출력하기 위한 유틸리티 함수들을 제공합니다.
 *
 * 이 네임스페이스의 함수들은 주로 디버그 빌드에서만 사용되며,
 * 릴리즈 빌드에서는 제거되거나 사용되지 않도록 설정할 수 있습니다.
 */
namespace Debug
{
	/**
	 * @brief 지정된 카테고리와 메시지를 화면 및 Output Log에 출력합니다.
	 *
	 * 이 함수는 메시지에 카테고리 접두사를 붙여 출력하여 메시지의 출처를 명확히 합니다.
	 * Output Log에는 `LogTemp` 카테고리의 `Warning` 수준으로 출력됩니다.
	 *
	 * @param Category 메시지의 분류를 나타내는 문자열입니다. (예: "Combat", "AI", "Movement")
	 * @param Message 화면과 Output Log에 출력할 실제 디버그 메시지입니다. FString::Printf를 사용하여 포매팅할 수 있습니다.
	 * @param Color 화면에 표시될 메시지의 색상입니다. (기본값: FColor::Yellow)
	 * @param Inkey 화면 메시지를 식별하는 고유 키입니다.
	 *              -1을 지정하면 매 호출마다 새로운 메시지가 출력됩니다.
	 *              특정 값을 지정하면 동일 키의 이전 메시지를 갱신합니다. (기본값: -1)
	 * @note 화면 메시지는 7초간 표시됩니다.
	 */
	static void Print(
		const FString& Category,
		const FString& Message,
		const FColor& Color = FColor::Yellow,
		int32 Inkey = -1)
	{
		if (GEngine)
		{
			const FString DebugMessage = FString::Printf(TEXT("[%s] %s"), *Category, *Message);
			GEngine->AddOnScreenDebugMessage(Inkey, 7.f, Color, DebugMessage);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMessage);
		}
	}

	/**
	 * @brief 메시지를 화면 및 Output Log에 출력합니다 (카테고리 없음).
	 *
	 * 이 함수는 메시지에 별도의 카테고리 없이 직접 메시지를 출력합니다.
	 * Output Log에는 `LogTemp` 카테고리의 `Warning` 수준으로 출력됩니다.
	 *
	 * @param Message 화면과 Output Log에 출력할 실제 디버그 메시지입니다.
	 * @param Color 화면에 표시될 메시지의 색상입니다. (기본값: FColor::MakeRandomColor() - 매 호출마다 임의 색상)
	 * @param Inkey 화면 메시지를 식별하는 고유 키입니다.
	 *              -1을 지정하면 매 호출마다 새로운 메시지가 출력됩니다.
	 *              특정 값을 지정하면 동일 키의 이전 메시지를 갱신합니다. (기본값: -1)
	 * @note 화면 메시지는 7초간 표시됩니다.
	 */
	static void Print(const FString& Message, const FColor& Color = FColor::MakeRandomColor(), int32 Inkey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Inkey, 7.f, Color, Message);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}

	/**
	 * @brief float 값을 제목과 함께 화면 및 Output Log에 출력합니다.
	 *
	 * 주로 수치 값을 실시간으로 확인해야 할 때 유용합니다.
	 * "제목: 값" 형식으로 출력됩니다.
	 * Output Log에는 `LogTemp` 카테고리의 `Warning` 수준으로 출력됩니다.
	 *
	 * @param FloatTitle 출력할 float 값의 제목입니다. (예: "PlayerSpeed", "HP")
	 * @param FloatValueToPrint 화면 및 Output Log에 출력할 실제 float 값입니다.
	 * @param key 화면 메시지를 식별하는 고유 키입니다.
	 *            -1을 지정하면 매 호출마다 새로운 메시지가 출력됩니다.
	 *            특정 값을 지정하면 동일 키의 이전 메시지를 갱신합니다. (기본값: -1)
	 * @param Color 화면에 표시될 메시지의 색상입니다. (기본값: FColor::MakeRandomColor() - 매 호출마다 임의 색상)
	 * @note 화면 메시지는 7초간 표시됩니다.
	 */
	static void Print(
		const FString& FloatTitle,
		float FloatValueToPrint,
		int32 key = -1,
		const FColor& Color = FColor::MakeRandomColor()
	)
	{
		if (GEngine)
		{
			const FString Message = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);

			GEngine->AddOnScreenDebugMessage(key, 7.f, Color, Message);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}

	/**
	 * @brief bool 값을 제목과 함께 화면 및 Output Log에 출력합니다.
	 *
	 * "제목: True/False" 형식으로 출력되어, boolean 값의 상태를 쉽게 확인할 수 있습니다.
	 * Output Log에는 `LogTemp` 카테고리의 `Warning` 수준으로 출력됩니다.
	 *
	 * @param BoolTitle 출력할 bool 값의 제목입니다. (예: "IsMoving", "HasAmmo")
	 * @param BoolValueToPrint 화면 및 Output Log에 출력할 실제 bool 값입니다. (True/False로 변환됨)
	 * @param key 화면 메시지를 식별하는 고유 키입니다.
	 *            -1을 지정하면 매 호출마다 새로운 메시지가 출력됩니다.
	 *            특정 값을 지정하면 동일 키의 이전 메시지를 갱신합니다. (기본값: -1)
	 * @param Color 화면에 표시될 메시지의 색상입니다. (기본값: FColor::MakeRandomColor() - 매 호출마다 임의 색상)
	 * @note 화면 메시지는 7초간 표시됩니다.
	 */
	static void Print(
		const FString& BoolTitle,
		bool BoolValueToPrint,
		int32 key = -1,
		const FColor& Color = FColor::MakeRandomColor()
	)
	{
		if (GEngine)
		{
			const FString BoolString = BoolValueToPrint ? TEXT("True") : TEXT("False");
			const FString Message = FString::Printf(TEXT("%s: %s"), *BoolTitle, *BoolString);

			GEngine->AddOnScreenDebugMessage(key, 7.f, Color, Message);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}
}
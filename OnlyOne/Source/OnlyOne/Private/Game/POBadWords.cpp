#include "game/POBadWords.h"
#include "Internationalization/Regex.h"

namespace
{
	const TArray<FString> GEnglishBadWords = {
		TEXT("fuck"), TEXT("shit"), TEXT("bitch"), TEXT("bastard"),
		TEXT("asshole"), TEXT("dick"), TEXT("pussy"), TEXT("slut"),
		TEXT("whore"), TEXT("cunt"), TEXT("faggot"), TEXT("retard"),
		TEXT("nigger"), TEXT("motherfucker"), TEXT("damn"), TEXT("crap"),
		TEXT("douche"), TEXT("prick"), TEXT("twat"),

		TEXT("fucking"), TEXT("fuckin"), TEXT("fuk"), TEXT("fux"),
		TEXT("shitty"), TEXT("bullshit"), TEXT("dipshit"),
		TEXT("cock"), TEXT("dildo"), TEXT("jerkoff"),
		TEXT("wanker"), TEXT("scumbag"), TEXT("hoe"),
	};
	
	const TArray<FString> GKoreanBadWords = {

		TEXT("시발"), TEXT("씨발"), TEXT("씨바"), TEXT("씹할"),
		TEXT("병신"), TEXT("븅신"), TEXT("ㅂㅅ"),
		TEXT("좆"), TEXT("존나"), TEXT("좆같"), TEXT("ㅈ같"),
		TEXT("새끼"), TEXT("섀끼"), TEXT("쌔끼"),
		TEXT("개새"), TEXT("개새끼"), TEXT("개년"), TEXT("개놈"),
		TEXT("개같"), TEXT("개지랄"), TEXT("개빡"),
		TEXT("지랄"), TEXT("ㅈㄹ"),

		TEXT("ㅅㅂ"), TEXT("ㅆㅂ"), TEXT("ㅅ발"),
		TEXT("ㅈ발"), TEXT("시바"), TEXT("씨빡"),
		TEXT("ㄱㅅㄲ"), TEXT("개쉐"), TEXT("개샊"),

		TEXT("꺼져"), TEXT("죽어"), TEXT("뒤져"),
		TEXT("또라이"), TEXT("돌아이"),
		TEXT("정신병자"), TEXT("미친"),
		TEXT("창녀"), TEXT("창놈"), TEXT("걸레"),
	};
	
	const TCHAR Delims[] = {
		TEXT(' '), TEXT('\t'), TEXT('\n'), TEXT('\r'),
		TEXT('.'), TEXT(','), TEXT('!'), TEXT('?'),
		TEXT(':'), TEXT(';'), TEXT('"'), TEXT('\''),
		TEXT('('), TEXT(')'), TEXT('{'), TEXT('}'),
		TEXT('['), TEXT(']'), TEXT('-'), TEXT('_'),
		TEXT('/'), TEXT('\\'), TEXT('|'), TEXT('+'),
		TEXT('=')
	};

	FORCEINLINE bool IsDelim(TCHAR C)
	{
		for (TCHAR D : Delims)
		{
			if (C == D) return true;
		}
		return false;
	}
	
	static FORCEINLINE FString Normalize(const FString& In)
	{
		FString Out = In;
		Out.ToLowerInline();
		Out = Out.TrimStartAndEnd();
		return Out;
	}
	
	static void Tokenize(const FString& In, TArray<FString>& OutTokens)
	{
		OutTokens.Reset();
		FString Current;
		Current.Reserve(16);

		for (TCHAR C : In)
		{
			if (IsDelim(C))
			{
				if (!Current.IsEmpty())
				{
					OutTokens.Add(MoveTemp(Current));
					Current.Empty(16);
				}
			}
			else
			{
				Current.AppendChar(C);
			}
		}
		if (!Current.IsEmpty())
		{
			OutTokens.Add(MoveTemp(Current));
		}
	}
}

bool POBadWords::ContainsProfanity(const FString& InText)
{
	const FString Norm = Normalize(InText);
	if (Norm.IsEmpty())
	{
		return false;
	}
	
	for (const FString& W : GEnglishBadWords)
	{
		const FString PatStr = FString::Printf(TEXT("\\b%s\\b"), *W);
		const FRegexPattern Pattern(PatStr);
		FRegexMatcher Matcher(Pattern, Norm);
		if (Matcher.FindNext())
		{
			return true;
		}
	}
	
	TArray<FString> Tokens;
	Tokenize(Norm, Tokens);
	for (const FString& Tok : Tokens)
	{
		for (const FString& Bad : GKoreanBadWords)
		{
			if (Tok.Contains(Bad))
			{
				return true;
			}
		}
	}

	return false;
}
#include "Map/Gimmick/POSlowMine.h"
#include "POGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemBlueprintLibrary.h" 
#include "GameplayEffect.h"                 
#include "GameplayEffectTypes.h"

APOSlowMine::APOSlowMine()
{
}

void APOSlowMine::OnGimmickComplete_Implementation(AActor* Target)
{
	Super::OnGimmickComplete_Implementation(Target);
}


void APOSlowMine::ActivateGimmick_Implementation(AActor* Target)
{
	Super::ActivateGimmick_Implementation(Target);

	if (UAbilitySystemComponent* ASC = GetASC(Target))
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GE_Slow,1.0f,ASC->MakeEffectContext());
		if (Spec.IsValid())
		{
			FGameplayEffectSpec& SpecRef = *Spec.Data.Get();
			SpecRef.SetDuration(5.0f,false);
			ASC->ApplyGameplayEffectSpecToSelf(SpecRef);
		}
	}

	OnGimmickComplete(Target);
}


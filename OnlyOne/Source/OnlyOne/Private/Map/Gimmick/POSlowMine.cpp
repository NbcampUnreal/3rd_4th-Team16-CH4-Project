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
	if (!HasAuthority())
	{
		return;
	}
	
	if (UAbilitySystemComponent* ASC = GetASC(Target))
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GE_Slow,1.f,ASC->MakeEffectContext());
		if (!Spec.IsValid())
		{
			return;
		}
		
		FGameplayEffectSpec& SpecRef = *Spec.Data.Get();
		SpecRef.SetDuration(5.0f, false);

		SpecRef.DynamicGrantedTags.AddTag(POGameplayTags::Shared_Status_Slow);
		ASC->ApplyGameplayEffectSpecToSelf(SpecRef);
	}

	OnGimmickComplete(Target);
}


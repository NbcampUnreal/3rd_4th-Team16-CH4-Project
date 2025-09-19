#include "Map/Gimmick/POSoundMine.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "POGameplayTags.h"

APOSoundMine::APOSoundMine()
{
	
}



void APOSoundMine::OnGimmickComplete_Implementation(AActor* Target)
{
	Super::OnGimmickComplete_Implementation(Target);
}


void APOSoundMine::ActivateGimmick_Implementation(AActor* Target)
{
	if (UAbilitySystemComponent* ASC = GetASC(Target))
	{
		FGameplayCueParameters P;
		P.SourceObject = this;
		P.Location = GetActorLocation();
		ASC->ExecuteGameplayCue(POGameplayTags::GameplayCue_GimmickTag_Sound, P);
	}
	
	OnGimmickComplete(Target);
}


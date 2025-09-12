#include "Map/Gimmick/POSoundMine.h"

APOSoundMine::APOSoundMine()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Root);
}

void APOSoundMine::OnGimmickComplete_Implementation(AActor* Target)
{
	Super::OnGimmickComplete_Implementation(Target);
}

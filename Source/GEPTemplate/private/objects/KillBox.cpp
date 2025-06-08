#include "objects/KillBox.h"

#include "allies/MainCharacter.h"
#include "components/HealthComponent.h"


AKillBox::AKillBox()
{
	
}

void AKillBox::Kill(AActor* OverlappedActor, AActor* OtherActor)
{
	if (auto MC = Cast<AMainCharacter>(OtherActor))
	{
		if (MC->HealthC) MC->HealthC->UpdateHealth(-MC->HealthC->MaxHealth);
	}
}

void AKillBox::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AKillBox::Kill);
}

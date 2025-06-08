#include "HYS/BlackMelee.h"

#include "components/HealthComponent.h"


ABlackMelee::ABlackMelee()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshC = GetMesh();
	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT(
			"/Game/HYS/ParagonMinions/Characters/Minions/Dusk_Minions/Meshes/Minion_Lane_Melee_Dusk.Minion_Lane_Melee_Dusk"));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }
}

void ABlackMelee::BeginPlay()
{
	Super::BeginPlay();
	
	if (MeleeHealthC)
	{
		MeleeHealthC->MaxHealth = InitialBlackMaxHealth;
		MeleeHealthC->CurrentHealth = InitialBlackMaxHealth;
	}
}

void ABlackMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

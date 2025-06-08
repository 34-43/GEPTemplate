#include "HYS/WhiteMelee.h"

#include "components/HealthComponent.h"


AWhiteMelee::AWhiteMelee()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshC = GetMesh();
	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT(
			"/Game/HYS/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Melee_Dawn.Minion_Lane_Melee_Dawn"));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }
}

void AWhiteMelee::BeginPlay()
{
	Super::BeginPlay();

	if (MeleeHealthC)
	{
		MeleeHealthC->MaxHealth = InitialWhiteMaxHealth;
		MeleeHealthC->CurrentHealth = InitialWhiteMaxHealth;
	}
}

void AWhiteMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

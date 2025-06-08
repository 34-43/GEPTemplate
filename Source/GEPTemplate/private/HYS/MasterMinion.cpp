#include "HYS/MasterMinion.h"

#include "components/HealthComponent.h"

AMasterMinion::AMasterMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshC = GetMesh();
	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT(
			"/Game/HYS/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Super_Dawn.Minion_Lane_Super_Dawn"));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }
}

void AMasterMinion::BeginPlay()
{
	Super::BeginPlay();

	if (MinionHealthC)
	{
		MinionHealthC->MaxHealth = InitialMaxHealth;
		MinionHealthC->CurrentHealth = InitialMaxHealth;
	}
}

void AMasterMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



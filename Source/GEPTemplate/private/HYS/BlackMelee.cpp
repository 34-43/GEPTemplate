#include "HYS/BlackMelee.h"


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
	
}

void ABlackMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

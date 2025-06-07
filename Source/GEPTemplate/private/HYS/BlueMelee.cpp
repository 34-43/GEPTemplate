#include "HYS/BlueMelee.h"


ABlueMelee::ABlueMelee()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MeshC = GetMesh();
	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT(
			"/Game/HYS/ParagonMinions/Characters/Minions/Dusk_Minions/Meshes/Minion_Lane_Melee_Core_Dusk.Minion_Lane_Melee_Core_Dusk"));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }
}

void ABlueMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlueMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

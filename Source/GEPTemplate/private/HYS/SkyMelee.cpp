#include "HYS/SkyMelee.h"


ASkyMelee::ASkyMelee()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshC = GetMesh();
	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT(
			"/Game/HYS/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Melee_Core_Dawn.Minion_Lane_Melee_Core_Dawn"));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }
}

void ASkyMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkyMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

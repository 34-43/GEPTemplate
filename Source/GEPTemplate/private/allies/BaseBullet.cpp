#include "allies/BaseBullet.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ABaseBullet::ABaseBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 설정
	CollC = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(CollC);
	CollC->SetCollisionProfileName(TEXT("BlockAll"));
	CollC->SetSphereRadius(12.5f);

	// 메시 설정
	MeshC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshC->SetupAttachment(CollC);
	MeshC->SetRelativeLocation(FVector(0, 0, -12.5f));
	MeshC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshC->SetRelativeScale3D(FVector(0.25f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereMesh.Succeeded()) { MeshC->SetStaticMesh(SphereMesh.Object); }

	// 투사체 움직임 설정
	ProjMoveC = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjMoveC->SetUpdatedComponent(CollC);
	ProjMoveC->InitialSpeed = 5000.0f;
	ProjMoveC->MaxSpeed = 5000.0f;
	ProjMoveC->bShouldBounce = true;
	ProjMoveC->Bounciness = 0.3f;

	// 액터 라이프타임 설정 예시
	// InitialLifeSpan = 2.0f;
}

void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();

	BeginPlaySetDestroyTimer();
}

void ABaseBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBullet::BeginPlaySetDestroyTimer()
{
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]() -> void
	{
		Destroy();
	}), 2.0f, false);
}

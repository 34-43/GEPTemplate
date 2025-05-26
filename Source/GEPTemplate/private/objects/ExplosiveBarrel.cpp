// ExplosiveBarrel.cpp

#include "objects/ExplosiveBarrel.h"
#include "components/InteractionComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "components/HealthComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("Object"));
	RootComponent = Mesh;

	// 상호작용 컴포넌트 설정
	InteractC = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	// 폭발력
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->SetupAttachment(RootComponent);
	RadialForce->Radius = 400.f;
	RadialForce->ImpulseStrength = 2000.f;
	RadialForce->bImpulseVelChange = true;
	RadialForce->bAutoActivate = false;
	RadialForce->bIgnoreOwningActor = true;

	// 파편 메시 6개 생성 및 자식으로 붙임
	Fragment0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fragment0"));
	Fragment0->SetupAttachment(Mesh);
	Fragment0->SetMobility(EComponentMobility::Movable);
	Fragment0->SetSimulatePhysics(false);
	Fragment0->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본은 충돌 끔
	
	Fragment1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fragment1"));
	Fragment1->SetupAttachment(Mesh);
	Fragment1->SetMobility(EComponentMobility::Movable);
	Fragment1->SetSimulatePhysics(false);
	Fragment1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Fragment2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fragment2"));
	Fragment2->SetupAttachment(Mesh);
	Fragment2->SetMobility(EComponentMobility::Movable);
	Fragment2->SetSimulatePhysics(false);
	Fragment2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Fragment3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fragment3"));
	Fragment3->SetupAttachment(Mesh);
	Fragment3->SetMobility(EComponentMobility::Movable);
	Fragment3->SetSimulatePhysics(false);
	Fragment3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Fragment4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fragment4"));
	Fragment4->SetupAttachment(Mesh);
	Fragment4->SetMobility(EComponentMobility::Movable);
	Fragment4->SetSimulatePhysics(false);
	Fragment4->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Fragment5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fragment5"));
	Fragment5->SetupAttachment(Mesh);
	Fragment5->SetMobility(EComponentMobility::Movable);
	Fragment5->SetSimulatePhysics(false);
	Fragment5->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void AExplosiveBarrel::Interact(AActor* Caller)
{
	if (!Caller) return;
	UE_LOG(LogTemp, Log, TEXT("!! 폭 * 8 !!"));
	Explode();
}

void AExplosiveBarrel::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (Mesh)
	{
		FVector CurrentLocation = Mesh->GetRelativeLocation();  // 현재 위치
		FVector Origin, BoxExtent;
		Mesh->GetLocalBounds(Origin, BoxExtent);

		// Z만 조절, 나머지는 그대로 유지
		CurrentLocation.Z = BoxExtent.Z;
		Mesh->SetRelativeLocation(CurrentLocation);
	}
}

void AExplosiveBarrel::Explode()
{
	// 이미 폭발한 경우 무시
	if (bHasExploded) return;
	bHasExploded = true;
	
	if (ExplosionEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	if (ExplosionSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	
	// 1. 메인 메시 숨기기 (또는 Destroy)
	Mesh->SetVisibility(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 2. 파편 활성화: 물리 켜고 충돌 켜기
	UStaticMeshComponent* Fragments[] = { Fragment0, Fragment1, Fragment2, Fragment3, Fragment4, Fragment5 };
	for (auto Fragment : Fragments)
	{
		Fragment->SetSimulatePhysics(true);
		Fragment->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Fragment->SetCollisionProfileName(TEXT("PhysicsActor"));
	}

	// 3. 폭발력 발사
	RadialForce->FireImpulse();

	FVector Origin = GetActorLocation();

	// 데미지 줄 액터들 중복 방지를 위한 Set
	TSet<AActor*> DamagedActors;

	TArray<FOverlapResult> Results;
	FCollisionShape Shape = FCollisionShape::MakeSphere(ExplosionRadius);
	if (GetWorld()->OverlapMultiByChannel(Results, Origin, FQuat::Identity, ECC_WorldDynamic, Shape))
	{
		for (auto& Hit : Results)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActor == this) continue;

			if (!DamagedActors.Contains(HitActor))
			{
				DamagedActors.Add(HitActor);

				if (UHealthComponent* HealthC = HitActor->FindComponentByClass<UHealthComponent>())
				{
					HealthC->UpdateHealth(-ExplosionDamage);
				}
			}
		}
	}

	// 4. 6초 후에 액터 삭제 예약
	GetWorldTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AExplosiveBarrel::OnDestroyTimerExpired,
		6.0f,
		false
	);
}

void AExplosiveBarrel::OnDestroyTimerExpired()
{
	Destroy();
}
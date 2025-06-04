// ExplosiveBarrel.cpp

#include "objects/ExplosiveBarrel.h"
#include "components/InteractionComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "components/HealthComponent.h"
#include "components/HealthFloatingComponent.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root로 박스 콜리전 생성
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollision"));
	BoxComp->SetBoxExtent(FVector(30.f, 30.f, 45.f));
	BoxComp->SetRelativeLocation(FVector(0.f, 0.f, BoxComp->GetScaledBoxExtent().Z));
	RootComponent = BoxComp;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("Object"));
	Mesh->SetupAttachment(RootComponent);

	// 상호작용 컴포넌트 설정
	InteractC = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	// 체력 컴포넌트 설정
	HealthC = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthC->MaxHealth = 24.0;
	DamageTicksRemaining = 7; // (0.5초 마다 총 6틱, 여유분 있음)
	DamagePerTick = 4.f;
	HealthFloatingC = CreateDefaultSubobject<UHealthFloatingComponent>(TEXT("HealthFloatingComponent"));
	HealthFloatingC->HideHealthBar();

	// 폭발력 (이펙트용)
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->SetupAttachment(Mesh);
	RadialForce->Radius = 400.f;
	RadialForce->ImpulseStrength = 1800.f;
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
	if (HealthC)
	{
		HealthC->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);
	}
	// UI 렌더링에 필요한 1P 컨트롤러 지정
	MainPlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void AExplosiveBarrel::Interact(AActor* Caller)
{
	if (AlertSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), AlertSound, GetActorLocation());
	InteractC->SetPower(false);
	if (!Caller || bHasExploded) return;
	// 체력 감소 타이머 시작
	HealthFloatingC->ShowHealthBar();
	GetWorldTimerManager().SetTimer(
		DamageTimerHandle,
		this,
		&AExplosiveBarrel::TickDamage,
		0.5f,
		true // 반복 호출
	);
}

void AExplosiveBarrel::Explode()
{
	// 이미 폭발한 경우 무시
	if (bHasExploded) return;
	bHasExploded = true;
	// 이펙트, 사운드
	if (ExplosionEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	if (ExplosionSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	// 1. 메인 메시 숨기기 (또는 Destroy)
	if (Mesh)
	{
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 2. 파편 활성화: 물리 켜고 충돌 켜기
	UStaticMeshComponent* Fragments[] = {Fragment0, Fragment1, Fragment2, Fragment3, Fragment4, Fragment5};
	for (auto Fragment : Fragments)
	{
		if (Fragment)
		{
			Fragment->SetSimulatePhysics(true);
			Fragment->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Fragment->SetCollisionProfileName(TEXT("PhysicsActor"));
		}
	}

	// 3. 폭발력 발사
	if (RadialForce)
	{
		RadialForce->FireImpulse();
	}

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

				if (UHealthComponent* OtherHealthC = HitActor->FindComponentByClass<UHealthComponent>())
				{
					OtherHealthC->UpdateHealth(-ExplosionDamage);
				}
			}
		}
	}

	// 4. 상호작용 비활성화, 체력바 숨김
	if (InteractC) InteractC->SetPower(false);
	if (HealthFloatingC) HealthFloatingC->HideHealthBar();
	// 5. 7초 후에 파편 삭제 예약
	GetWorldTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AExplosiveBarrel::OnDestroyTimerExpired,
		7.0f,
		false
	);
}

void AExplosiveBarrel::OnDestroyTimerExpired()
{
	Destroy();
}

void AExplosiveBarrel::OnHealthChanged(int32 NewHealth, int32 MaxHealth)
{
	HealthFloatingC->ShowHealthBar();
	if (NewHealth <= 0 && !bHasExploded)
	{
		HealthFloatingC->HideHealthBar();
		// 랜덤 시간 후 폭발
		float RandomDelay = FMath::FRandRange(0.05f, 0.3f);
		GetWorldTimerManager().SetTimer(
			ExplosionTimerHandle,
			this,
			&AExplosiveBarrel::Explode,
			RandomDelay,
			false
		);
	}
}

void AExplosiveBarrel::TickDamage()
{
	if (HealthC)
	{
		HealthC->UpdateHealth(-DamagePerTick);
	}

	DamageTicksRemaining--;
	if (DamageTicksRemaining <= 0)
	{
		GetWorldTimerManager().ClearTimer(DamageTimerHandle);
	}
}

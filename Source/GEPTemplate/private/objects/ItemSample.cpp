#include "objects/ItemSample.h"
#include "Components/SphereComponent.h"
#include "allies/MainCharacter.h"
#include "components/HealthComponent.h"
#include "components/StaminaComponent.h"

AItemSample::AItemSample()
{
	PrimaryActorTick.bCanEverTick = false;

	// 콜리전 컴포넌트 (오버랩 감지용)
	CollC = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollC->InitSphereRadius(50.f);
	SetRootComponent(CollC);

	// 메시 컴포넌트 (시각적 표현용)
	MeshC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshC->SetupAttachment(RootComponent);
	MeshC->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
	MeshC->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시 자체는 충돌 비활성화

	// 오버랩 이벤트 연결
	CollC->OnComponentBeginOverlap.AddDynamic(this, &AItemSample::OnOverlapBegin);
}

void AItemSample::BeginPlay()
{
	Super::BeginPlay();
}

void AItemSample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemSample::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CustomMaterial && MeshC)
	{
		MeshC->SetMaterial(0, CustomMaterial);
	}
}

void AItemSample::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
							   AActor* OtherActor,
							   UPrimitiveComponent* OtherComp,
							   int32 OtherBodyIndex,
							   bool bFromSweep,
							   const FHitResult& SweepResult)
{
	AMainCharacter* MainChar = Cast<AMainCharacter>(OtherActor);
	if (MainChar)
	{
		Use(MainChar); // 사용
	}
}


void AItemSample::Use(AMainCharacter* TargetCharacter)
{
	if (!TargetCharacter) return;

	TargetCharacter->HealthC->UpdateHealth(HealthDelta);
	TargetCharacter->StaminaC->UpdateStamina(StaminaDelta);
	TargetCharacter->ManageGold(GoldDelta);

	Destroy(); // 아이템 제거
}
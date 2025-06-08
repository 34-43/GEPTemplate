#include "objects/ItemSample.h"
#include "Components/SphereComponent.h"
#include "allies/MainCharacter.h"
#include "components/HealthComponent.h"
#include "components/StaminaComponent.h"
#include "Kismet/GameplayStatics.h"

AItemSample::AItemSample()
{
	PrimaryActorTick.bCanEverTick = true;

	// 콜리전 컴포넌트 (오버랩 감지용)
	CollC = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollC->InitSphereRadius(40.f);
	CollC->SetRelativeLocation(FVector(0.f, 0.f, CollC->GetScaledSphereRadius()));
	SetRootComponent(CollC);
	CollC->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 메시 컴포넌트 (시각적 표현용)
	MeshC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshC->SetupAttachment(RootComponent);
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
	// 일정 속도로 회전
	FRotator RotationDelta(0.f, 120.f * DeltaTime, 0.f);
	MeshC->AddRelativeRotation(RotationDelta);
}

void AItemSample::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CustomMaterial && MeshC)
	{
		MeshC->SetMaterial(0, CustomMaterial);
	}
	// Yaw 방향만 랜덤 회전
	const float RandomYaw = FMath::FRandRange(0.f, 360.f);
	FRotator RandomRotation(0.f, RandomYaw, 0.f);
	MeshC->SetRelativeRotation(RandomRotation);
}

void AItemSample::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
							   AActor* OtherActor,
							   UPrimitiveComponent* OtherComp,
							   int32 OtherBodyIndex,
							   bool bFromSweep,
							   const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Item Overlapped with: %s"), *OtherActor->GetName());

	AMainCharacter* MainChar = Cast<AMainCharacter>(OtherActor);
	if (MainChar)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapped actor is MainCharacter. Applying item effect."));
		Use(MainChar);
	}
}


void AItemSample::Use(AMainCharacter* TargetCharacter)
{
	if (!TargetCharacter) return;

	TargetCharacter->HealthC->UpdateHealth(HealthDelta);
	TargetCharacter->StaminaC->UpdateStamina(StaminaDelta);
	TargetCharacter->ManageGold(GoldDelta);

	if (UseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, UseSound, GetActorLocation());
	}
	
	Destroy(); // 아이템 제거
}
#include "enemies/bosses/BaseBossPart.h"

#include "AIController.h"
#include "GEPTemplate.h"
#include "allies/MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "components/HealthComponent.h"
#include "components/CombatComponent.h"
#include "components/FocusedComponent.h"
#include "Components/ModelComponent.h"
#include "enemies/bosses/BaseBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


ABaseBossPart::ABaseBossPart()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 세팅 --- 피집중C, 전투C
	FocusedC = CreateDefaultSubobject<UFocusedComponent>("FocusedComponent");
	FocusedC->SetupWidgetAttachment(GetRootComponent(), NAME_None);
	CombatC = CreateDefaultSubobject<UCombatComponent>("CombatComponent");

	// 팩토리 세팅 --- 충격파F, 폭발F
	static ConstructorHelpers::FClassFinder<AShockwave> ShockwaveBP(
		TEXT("/Game/Blueprints/BP_Shockwave.BP_Shockwave_C"));
	if (ShockwaveBP.Succeeded()) { ShockwaveF = ShockwaveBP.Class; }
	static ConstructorHelpers::FClassFinder<AExplosion> ExplosionBP(
		TEXT("/Game/Blueprints/BP_Explosion.BP_Explosion_C"));
	if (ExplosionBP.Succeeded()) { ExplosionF = ExplosionBP.Class; }
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DamagedEffect(
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DamagedEffect.Succeeded()) { DamagedFxF = DamagedEffect.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> DamagedSound(
		TEXT("/Game/Features/BaseBallBatSound/bat_machine.bat_machine"));
	if (DamagedSound.Succeeded()) { DamagedSfxF = DamagedSound.Object; }
}

void ABaseBossPart::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어1 캐시 생성
	Player = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// 충돌 델리게이트 연결
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABaseBossPart::OnCapsuleHit);

	// 전투와 체력 델리게이트 연결
	CombatC->OnDamaged.AddDynamic(this, &ABaseBossPart::HandleDamaged);
}

void ABaseBossPart::OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 FVector NormalImpulse, const FHitResult& Hit)
{
	// 지오메트리 브러시에 특정적으로 동작하는 충돌 조건
	if (!bIsFlying && bBeforePound && OtherComp->IsA(UModelComponent::StaticClass()))
	{
		bBeforePound = false;
		if (bIsExplosive)
		{
			SpawnExplosion(25, 2000.0f, Hit.ImpactPoint);
		}
		else
		{
			SpawnShockwave(25, 2000.0f, Hit.ImpactPoint);
		}
		GetWorldTimerManager().SetTimer(BaseBossPartTimerHandle, [this]()
		{
			OnPoundCompleted.ExecuteIfBound();
		}, 1.0f, false);
	}
}

void ABaseBossPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 전체 이동을 Tick 이 제어
	TickMovement(DeltaTime);
}

void ABaseBossPart::TickMovement(float DeltaTime)
{
	// 제어 중이 아니면 Tick 이동 없이 종료
	if (!bIsFlying) return;

	// 파츠 자신의 위치와, 목적지까지의 현재 거리 계산
	const FVector CurrentLocation = GetActorLocation();
	const float RemainingDistance = FVector::Dist(CurrentLocation, FlyTargetLocation);

	// 도착을 허용할 오차(Acceptance) 범위 검사
	if (RemainingDistance <= FlyAcceptance)
	{
		// 도착 시 제어 해제, 속도 제거, 델리게이트 실행
		bIsFlying = false;
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		if (!OnFlyCompleted.ExecuteIfBound()) PRINT_LOG(TEXT("OnFlyCompleted Execution Failed."));
		return;
	}

	// 출발 시점 기준으로 현재 남은 거리의 비율을 사용해, 설정된 평균 속도의 1.9배에서 0.1배까지 선형적으로 감속
	const float Speed = FlyAverageSpeed * (1.8 * (RemainingDistance / FlyInitialDistance) + 0.1);
	const FVector NormalDirection = (FlyTargetLocation - CurrentLocation).GetSafeNormal();

	// 방향과 속도 둘 다 가진 Velocity 를 직접 지정하여 이동
	GetCharacterMovement()->Velocity = NormalDirection * Speed;
}

void ABaseBossPart::HandleDamaged()
{
	if (BaseBossCache) BaseBossCache->TotalHealthC->UpdateHealth(-FMath::RandRange(4,8));
	PRINT_LOG(TEXT("현재 보스 체력: %d"), BaseBossCache->TotalHealthC->CurrentHealth);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedFxF, GetActorLocation());
	UGameplayStatics::SpawnSound2D(GetWorld(), DamagedSfxF);
}

void ABaseBossPart::FlyToLocation(const FVector& Location, const float AverageSpeed,
                                  const FOnActionCompleted& OnCompleted)
{
	// 필요한 전역 값 설정
	FlyTargetLocation = Location;
	FlyInitialDistance = FVector::Dist(GetActorLocation(), FlyTargetLocation);
	bIsFlying = true;
	OnFlyCompleted = OnCompleted;
	FlyAverageSpeed = AverageSpeed;

	// 캐릭터 이동 컴포넌트에서 관련한 값을 '비행' 가능하도록 설정
	UCharacterMovementComponent* MoveC = GetCharacterMovement();
	MoveC->SetMovementMode(MOVE_Flying);
	MoveC->GravityScale = 0.f;
	MoveC->Velocity = FVector::ZeroVector;
}

void ABaseBossPart::PoundStraight(const FOnActionCompleted& OnCompleted)
{
	// 콜백 보관
	OnPoundCompleted = OnCompleted;

	// 충돌 플래그 활성화
	bBeforePound = true;

	// 안티시페이션 동작
	const float UpImpulse = 900.0f;
	LaunchCharacter(FVector(0, 0, UpImpulse), false, true);

	// 대기시간 이후 낙하 호출
	GetWorldTimerManager().SetTimer(BaseBossPartTimerHandle, [this]()
	{
		UCharacterMovementComponent* CharMoveC = GetCharacterMovement();
		CharMoveC->GravityScale = 10.0f;
		LaunchCharacter(FVector(0, 0, -1200.0f), false, true);
		CharMoveC->SetMovementMode(MOVE_Falling);
	}, 0.3f, false);
}

void ABaseBossPart::SpawnShockwave(int32 Damage, float ExpansionSpeed, FVector WorldLocation)
{
	AShockwave* SW = GetWorld()->SpawnActor<AShockwave>(ShockwaveF, WorldLocation, FRotator::ZeroRotator);
	SW->DamageAmount = Damage;
	SW->ExpansionSpeed = ExpansionSpeed;
}

void ABaseBossPart::SpawnExplosion(int32 Damage, float ExplosionRadius, FVector WorldLocation)
{
	AExplosion* EP = GetWorld()->SpawnActor<AExplosion>(ExplosionF, WorldLocation, FRotator::ZeroRotator);

	// AExplosion* EP = GetWorld()->SpawnActorDeferred<AExplosion>(ExplosionF, FTransform(FRotator::ZeroRotator, WorldLocation));
	// EP->DamageAmount = Damage;
	// EP->ExplosionRadius = ExplosionRadius;
	// EP->FinishSpawning(FTransform(FRotator::ZeroRotator, WorldLocation));
}

void ABaseBossPart::SetBaseBossCache(ABaseBoss* OwningBoss)
{
	BaseBossCache = OwningBoss;
}

void ABaseBossPart::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		if (AAIController* AC = Cast<AAIController>(GetController()))
		{
			AC->ReceiveMoveCompleted.RemoveDynamic(this, &ABaseBossPart::HandleMoveCompleted);
		}
		OnFlyCompleted.ExecuteIfBound();
	}
}

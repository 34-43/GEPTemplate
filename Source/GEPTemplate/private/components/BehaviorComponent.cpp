#include "components/BehaviorComponent.h"

#include "allies/MainCharacter.h"
#include "components/CombatComponent.h"
#include "components/HealthComponent.h"
#include "enemies/BaseEnemy.h"
#include "enemies/WalkingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "components/CombatComponent.h"

UBehaviorComponent::UBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass());
	Target = Cast<AMainCharacter>(actor);
	Me = Cast<ABaseEnemy>(GetOwner());

	// GetCharacterMovement()->bOrientRotationToMovement = true; //메시를 이동방향으로 계속 갱신하는 기능 사용
}


void UBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 상태 출력용 타이머
	LogTime += DeltaTime;
	if (LogTime >= 1.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy State: %s"), *UEnum::GetValueAsString(mState));
		LogTime = 0.0f;
	}

	TimeSinceLastAction += DeltaTime;

	if (!Me || !Target) return;

	switch (mState)
	{
	case EEnemyState::Idle: TickIdle();
		break;
	case EEnemyState::Move: TickMove();
		break;
	case EEnemyState::Battle: TickBattle();
		break;
	case EEnemyState::Flee: TickFlee();
		break;
	case EEnemyState::Wander: TickWander();
		break;
	case EEnemyState::Dead:
		break;
	}
}

void UBehaviorComponent::SetState(EEnemyState State)
{
	mState = State;
}

float UBehaviorComponent::GetDistanceFromTarget()
{
	return (Target->GetActorLocation() - Me->GetActorLocation()).Size();
}

void UBehaviorComponent::TickIdle()
{
	float distance = GetDistanceFromTarget();

	if (distance < StartDistance) // 사정거리 안에 들어오면
	{
		if (IsLowHealth())
		{
			SetState(EEnemyState::Flee);
		}
		else
		{
			SetState(EEnemyState::Move);
		}
	}
}

bool UBehaviorComponent::IsLowHealth()
{
	UHealthComponent* health = Me->FindComponentByClass<UHealthComponent>();

	if (health && health->MaxHealth > 0)
	{
		float hpRatio = static_cast<float>(health->CurrentHealth) / static_cast<float>(health->MaxHealth);

		if (hpRatio <= FleeRate)
		{
			// 0.1% 확률로만 도망가게 설정
			if (FMath::FRand() <= 0.001) // 0.0 ~ 1.0 사이 float 난수
			{
				return true;
			}
		}
	}
	return false;
}

void UBehaviorComponent::TickMove()
{
	FVector dir = Target->GetActorLocation() - Me->GetActorLocation();
	float distance = dir.Size();

	if (distance <= StartDistance && distance >= StopDistance)
	{
		float normalized = distance / StartDistance; // 0~1 범위로 정규화
		float alpha = 3.0f; // 민감도 계수 (실험 필요)
		float speedScale = 1.0f - FMath::Exp(-alpha * normalized); // 지수형 비례 속도

		speedScale = FMath::Clamp(speedScale, 0.1f, 1.0f); // 너무 작거나 클 경우 방지

		Me->AddMovementInput(dir.GetSafeNormal(), speedScale);
	}

	if (distance <= StopDistance)
	{
		SetState(EEnemyState::Battle);
	}
}

void UBehaviorComponent::TickBattle(){}

void UBehaviorComponent::TickFlee()
{
	FVector fleeDir = Me->GetActorLocation() - Target->GetActorLocation();
	float distance = fleeDir.Size();
	// 도망 성공 (충분히 멀어짐)
	if (distance >= SafeDistance)
	{
		mState = EEnemyState::Idle;
		CurrentFleeDirection = FVector::ZeroVector;
		return;
	}

	FleeMoveCooldown += GetWorld()->DeltaTimeSeconds;

	// 처음 도망 시작 시 → 정확히 반대 방향으로 설정
	if (CurrentFleeDirection.IsZero())
	{
		CurrentFleeDirection = fleeDir.GetSafeNormal(); // 처음엔 직선 방향
	}

	// 3초마다 방향 갱신 (지그재그)
	if (FleeMoveCooldown > FleeCooldownTime)
	{
		CurrentFleeDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(fleeDir.GetSafeNormal(), FleeAngle);
		FleeMoveCooldown = 0.0f;
	}

	float fleeSpeedScale = 0.3f; // 0.0 ~ 1.0 사이 (값이 낮을수록 느림)
	Me->AddMovementInput(CurrentFleeDirection, fleeSpeedScale);
}

void UBehaviorComponent::TickWander()
{
	if (!Me || !Target) return;

	float distance = GetDistanceFromTarget();
	if (distance > StopDistance)
	{
		SetState(EEnemyState::Move);
		return;
	}

	WanderElapsedTime += GetWorld()->DeltaTimeSeconds;

	// 배회 시간 끝났으면 전투로 복귀
	if (WanderElapsedTime >= MaxWanderDuration)
	{
		SetState(EEnemyState::Battle);
		return;
	}

	// 각도 증가
	WanderAngle += WanderDirectionSign * WanderSpeed * GetWorld()->DeltaTimeSeconds;
	if (WanderAngle >= 360.0f) WanderAngle -= 360.0f;
	if (WanderAngle < 0.0f) WanderAngle += 360.0f;

	// 이동 방향 계산
	FVector center = Target->GetActorLocation();
	float angleRad = FMath::DegreesToRadians(WanderAngle);
	FVector offset = FVector(FMath::Cos(angleRad), FMath::Sin(angleRad), 0) * WanderRadius;
	FVector newLocation = center + offset;

	FVector toCenter = (Target->GetActorLocation() - Me->GetActorLocation()).GetSafeNormal(); // 플레이어 방향
	FVector desiredDir = (newLocation - Me->GetActorLocation()).GetSafeNormal(); // 목표 위치 방향
	FVector moveDir = desiredDir - FVector::DotProduct(desiredDir, toCenter) * toCenter;
	moveDir = moveDir.GetSafeNormal();

	Me->AddMovementInput(moveDir, 0.1f); // ← 천천히 원형 이동

	// 적이 항상 플레이어 쳐다보게 회전
	FRotator lookRot = UKismetMathLibrary::FindLookAtRotation(Me->GetActorLocation(), center);
	Me->SetActorRotation(lookRot);
}

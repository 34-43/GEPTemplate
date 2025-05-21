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

	if (Me)
	{
		MyCombatC = Me->FindComponentByClass<UCombatComponent>();
	}
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
	case EEnemyState::Idle:
		TickIdle();
		break;
	case EEnemyState::Move:
		TickMove();
		break;
	case EEnemyState::Battle:
		TickBattle();
		break;
	case EEnemyState::Flee:
		TickFlee();
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
			return true;
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
		Me->AddMovementInput(dir.GetSafeNormal());
	}

	if (distance <= StopDistance)
	{
		SetState(EEnemyState::Battle);
	}
}

void UBehaviorComponent::TickBattle()
{
	// 기본적으로 방어, 간헐적으로 공격 시도, 플레이어 공격 감지 시 패링 시도
	if (IsLowHealth())
	{
		SetState(EEnemyState::Flee);
	}
	else
	{
		float distance = GetDistanceFromTarget();

		if (distance >= StopDistance)
		{
			SetState(EEnemyState::Idle);
		}
		else
		{
			if (MyCombatC)
			{
				if (TimeSinceLastAction >= ActionCooldown)
				{
					int32 totalRate = AttackRate + BlockRate + ParryRate;
					int32 rand = FMath::RandRange(1, totalRate); // 1부터 총합 사이에서 랜덤값

					if (rand <= AttackRate)
					{
						// 공격 횟수 결정 (1~4)
						MaxComboCount = FMath::RandRange(1, 4);
						CurrentComboIndex = 0;

						// 첫 공격 시작
						PerformComboAttack();
					}
					else if (rand <= AttackRate + BlockRate)
					{
						UE_LOG(LogTemp, Warning, TEXT("Block Attack"));
						// CombatC->Block();
					}
					else
					{
						MyCombatC->Parry();
					}

					TimeSinceLastAction = 0;
				}
			}
		}
	}
}

void UBehaviorComponent::PerformComboAttack()
{
	if (!MyCombatC) return;

	if (CurrentComboIndex < MaxComboCount)
	{
		MyCombatC->Attack(); // 한 번 공격
		CurrentComboIndex++;

		// 다음 공격 예약 (0.4초 간격 등으로 조절 가능)
		GetWorld()->GetTimerManager().SetTimer(
			ComboAttackTimerHandle,
			this,
			&UBehaviorComponent::PerformComboAttack,
			0.4f, // 공격 간격
			false
		);
	}
}


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

	Me->AddMovementInput(CurrentFleeDirection);
}

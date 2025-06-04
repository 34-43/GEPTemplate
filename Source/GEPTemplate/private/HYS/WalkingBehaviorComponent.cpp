#include "HYS/WalkingBehaviorComponent.h"

#include "allies/MainCharacter.h"
#include "components/CombatComponent.h"
#include "components/HealthComponent.h"
#include "enemies/BaseEnemy.h"
#include "Kismet/KismetMathLibrary.h"


UWalkingBehaviorComponent::UWalkingBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UWalkingBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Me)
	{
		MyCombatC = Me->FindComponentByClass<UCombatComponent>();
	}
}


void UWalkingBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWalkingBehaviorComponent::TickBattle()
{
	float distance = GetDistanceFromTarget();

	if (distance >= StopDistance)
	{
		SetState(EEnemyState::Idle);
		return;
	}

	if (MyCombatC && TimeSinceLastAction >= ActionCooldown)
	{
		if (IsLowHealth())
		{
			SetState(EEnemyState::Flee);
			return;
		}

		int32 behaviorRand = FMath::RandRange(1, 100); // 1~100 사이 랜덤값

		if (behaviorRand <= 30)
		{
			// 배회 상태 진입 시 방향 초기화
			WanderDirection = FVector::ZeroVector;
			WanderElapsedTime = 0.0f;
			WanderAngle = 0.0f;

			// 방향 랜덤 설정 (1 또는 -1)
			WanderDirectionSign = (FMath::RandBool() ? 1 : -1);

			SetState(EEnemyState::Wander);
		}
		else
		{
			int32 totalRate = AttackRate + ParryRate;
			int32 rand = FMath::RandRange(1, totalRate); // 1부터 총합 사이에서 랜덤값

			FVector center = Target->GetActorLocation();
			FRotator lookRot = UKismetMathLibrary::FindLookAtRotation(Me->GetActorLocation(), center);
			Me->SetActorRotation(lookRot);
			
			if (rand <= AttackRate)
			{
				// 공격 횟수 결정 (1~4)
				MaxComboCount = FMath::RandRange(1, 4);
				CurrentComboIndex = 0;

				// 첫 공격 시작
				PerformComboAttack();
			}
			else // if (rand <= AttackRate + ParryRate)
			{
				MyCombatC->Parry();
			}
		}

		TimeSinceLastAction = 0;
	}
}

void UWalkingBehaviorComponent::PerformComboAttack()
{
	if (!MyCombatC || !Me) return;

	UHealthComponent* health = Me->FindComponentByClass<UHealthComponent>();
	if (!health || health->CurrentHealth <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboAttackTimerHandle); // 타이머 제거
		return;
	}
	
	if (CurrentComboIndex < MaxComboCount)
	{
		MyCombatC->Attack(); // 한 번 공격
		CurrentComboIndex++;

		// 다음 공격 예약 (0.4초 간격 등으로 조절 가능)
		GetWorld()->GetTimerManager().SetTimer(
			ComboAttackTimerHandle,
			this,
			&UWalkingBehaviorComponent::PerformComboAttack,
			0.4f, // 공격 간격
			false
		);
	}
}

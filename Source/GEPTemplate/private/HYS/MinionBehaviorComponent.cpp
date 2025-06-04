#include "HYS/MinionBehaviorComponent.h"

#include "allies/MainCharacter.h"
#include "components/HealthComponent.h"
#include "enemies/BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYS/MinionCombatComponent.h"
#include "Kismet/KismetMathLibrary.h"


UMinionBehaviorComponent::UMinionBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UMinionBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Me)
	{
		MyCombatC = Me->FindComponentByClass<UMinionCombatComponent>();
	}
}


void UMinionBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMinionBehaviorComponent::TickBattle()
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
				// 공격 시작
				if (!MyCombatC || !Me) return;

				UHealthComponent* health = Me->FindComponentByClass<UHealthComponent>();
				if (!health || health->CurrentHealth <= 0)
				{
					GetWorld()->GetTimerManager().ClearTimer(ComboAttackTimerHandle); // 타이머 제거
					return;
				}

				PerformAttack();
				// MyCombatC->Attack(); // 한 번 공격
			}
			else // if (rand <= AttackRate + ParryRate)
			{
				MyCombatC->Parry();
			}
		}

		TimeSinceLastAction = 0;
	}
}

void UMinionBehaviorComponent::PerformAttack()
{
	if (!MyCombatC || !Me) return;

	UHealthComponent* health = Me->FindComponentByClass<UHealthComponent>();
	if (!health || health->CurrentHealth <= 0)
	{
		return;
	}
	
	// 이동 잠금
	ACharacter* Char = Cast<ACharacter>(Me);
	if (Char && Char->GetCharacterMovement())
	{
		Char->GetCharacterMovement()->DisableMovement();
	}

	// 공격 실행 (몽타주 or 애니메이션 시퀀스)
	MyCombatC->Attack();

	// 공격이 끝난 후 이동 다시 활성화 (애니메이션 시간 기준으로 타이머 설정)
	const float AttackDuration = 2.0f;
	GetWorld()->GetTimerManager().SetTimer(
		AttackEndTimerHandle,
		this,
		&UMinionBehaviorComponent::OnAttackEnd,
		AttackDuration,
		false
	);
}

void UMinionBehaviorComponent::OnAttackEnd()
{
	ACharacter* Char = Cast<ACharacter>(Me);
	if (Char && Char->GetCharacterMovement())
	{
		Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
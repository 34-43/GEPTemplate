#include "HYS/MeleeBehaviorComponent.h"

#include "allies/MainCharacter.h"
#include "components/HealthComponent.h"
#include "enemies/BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYS/MeleeCombatComponent.h"
#include "Kismet/KismetMathLibrary.h"


UMeleeBehaviorComponent::UMeleeBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UMeleeBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Me)
	{
		MyCombatC = Me->FindComponentByClass<UMeleeCombatComponent>();
	}
}


void UMeleeBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMeleeBehaviorComponent::TickBattle()
{
	float distance = GetDistanceFromTarget();

	if (distance >= StopDistance)
	{
		UE_LOG(LogHAL, Log, TEXT("stop dis"));
		SetState(EEnemyState::Idle);
		return;
	}
	
	if (MyCombatC && TimeSinceLastAction >= ActionCooldown && Me && Target)
	{
		if (IsLowHealth())
		{
			UE_LOG(LogHAL, Log, TEXT("low health"));
			SetState(EEnemyState::Flee);
			return;
		}

		int32 behaviorRand = FMath::RandRange(1, 100); // 1~100 사이 랜덤값

		if (behaviorRand <= WanderExecuteRate)
		{
			UE_LOG(LogHAL, Log, TEXT("wander"));

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
			UE_LOG(LogHAL, Log, TEXT("attack"));

			int32 totalRate = AttackRate + ParryRate;
			int32 rand = FMath::RandRange(1, totalRate); // 1부터 총합 사이에서 랜덤값

			FVector center = Target->GetActorLocation();
			FRotator lookRot = UKismetMathLibrary::FindLookAtRotation(Me->GetActorLocation(), center);
			Me->SetActorRotation(lookRot);

			if (rand <= AttackRate)
			{
				// 공격 시작
				if (!MyCombatC || !Me) return;

				PerformAttack();
			}
			else // if (rand <= AttackRate + ParryRate)
			{
				MyCombatC->Parry();
			}
		}

		TimeSinceLastAction = 0;
	}
}

void UMeleeBehaviorComponent::PerformAttack()
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

	// todo: 이 부분 오류날 확률 높음
	// 공격이 끝난 후 이동 다시 활성화 (애니메이션 시간 기준으로 타이머 설정)
	const float AttackDuration = 4.0f;
	GetWorld()->GetTimerManager().SetTimer(
		AttackEndTimerHandle,
		this,
		&UMeleeBehaviorComponent::OnAttackEnd,
		AttackDuration,
		false
	);
}

void UMeleeBehaviorComponent::OnAttackEnd()
{
	ACharacter* Char = Cast<ACharacter>(Me);
	if (Char && Char->GetCharacterMovement())
	{
		Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

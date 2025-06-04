#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BehaviorComponent.generated.h"

class UCombatComponent;

UENUM(Blueprintable)
enum class EEnemyState:uint8
{
	Idle,
	Move,
	Battle,
	Flee,
	Wander,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY()	EEnemyState mState = EEnemyState::Idle;

	void SetState(EEnemyState State);
	void TickIdle();
	bool IsLowHealth();
	void TickMove();
	virtual void TickBattle();
	void TickFlee();
	void TickWander();

	UPROPERTY()	class AMainCharacter* Target;
	UPROPERTY() class ABaseEnemy* Me;
	
	UPROPERTY(EditAnywhere, Category="Condition") float StopDistance = 250.0f;
	UPROPERTY(EditAnywhere, Category="Condition") float StartDistance = 1500.0f;
	UPROPERTY(EditAnywhere, Category="Condition") float SafeDistance = 2000.0f;
	UPROPERTY(EditAnywhere, Category="Condition") float FleeRate = 0.3f;
	
	UPROPERTY(EditAnywhere, Category="Flee") FVector CurrentFleeDirection = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category="Flee") float FleeCooldownTime = 1.0f;
	UPROPERTY(EditAnywhere, Category="Flee") float FleeAngle = 60.0f;

	UPROPERTY(EditAnywhere, Category="Battle") float ActionCooldown = 3.0f;
	UPROPERTY(EditAnywhere, Category="Battle") float AttackRate = 75.0f;
	UPROPERTY(EditAnywhere, Category="Battle") float ParryRate = 25.0f;
	UPROPERTY(EditAnywhere, Category="Battle") int CurrentComboIndex;
	UPROPERTY(EditAnywhere, Category="Battle") int MaxComboCount;

	UPROPERTY(EditAnywhere, Category="Wander") float MaxWanderDuration = 3.0f; // 배회 시간 (초)
protected:
	float GetDistanceFromTarget();
	
	float LogTime = 0.0f;
	float FleeMoveCooldown = 0.0f;
	float TimeSinceLastAction = 0.0f;
	FTimerHandle ComboAttackTimerHandle;
	
	FVector WanderDirection = FVector::ZeroVector;
	float WanderElapsedTime = 0.0f;
	float WanderAngle = 0.0f;           // 현재 각도
	float WanderRadius = 170.0f;        // 플레이어 기준 반지름
	float WanderSpeed = 90.0f;          // 초당 회전 각도 (도 단위)
	int32 WanderDirectionSign = 1; // +1이면 시계방향, -1이면 반시계방향
};

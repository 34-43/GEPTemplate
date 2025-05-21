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
	void TickBattle();
	void PerformComboAttack();
	void TickFlee();

	UPROPERTY()	class AMainCharacter* Target;
	UPROPERTY() class ABaseEnemy* Me;
	
	UPROPERTY()	UCombatComponent* MyCombatC;
	
	UPROPERTY(EditAnywhere, Category="Condition") float StopDistance = 150.0f;
	UPROPERTY(EditAnywhere, Category="Condition") float StartDistance = 1500.0f;
	UPROPERTY(EditAnywhere, Category="Condition") float SafeDistance = 2000.0f;
	UPROPERTY(EditAnywhere, Category="Condition") float FleeRate = 0.3f;
	
	UPROPERTY(EditAnywhere, Category="Flee") FVector CurrentFleeDirection = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category="Flee") float FleeCooldownTime = 1.0f;
	UPROPERTY(EditAnywhere, Category="Flee") float FleeAngle = 60.0f;

	UPROPERTY(EditAnywhere, Category="Battle") float ActionCooldown = 3.0f;
	UPROPERTY(EditAnywhere, Category="Battle") float AttackRate = 60.0f;
	UPROPERTY(EditAnywhere, Category="Battle") float BlockRate = 25.0f;
	UPROPERTY(EditAnywhere, Category="Battle") float ParryRate = 15.0f;
	UPROPERTY(EditAnywhere, Category="Battle") int CurrentComboIndex;
	UPROPERTY(EditAnywhere, Category="Battle") int MaxComboCount;
	
private:
	float GetDistanceFromTarget();
	
	float LogTime = 0.0f;
	float FleeMoveCooldown = 0.0f;
	float TimeSinceLastAction = 0.0f;
	FTimerHandle ComboAttackTimerHandle;
};

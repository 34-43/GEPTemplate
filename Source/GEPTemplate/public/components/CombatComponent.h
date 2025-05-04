#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	IdleMoving,
	BufferTime,
	Attacking,
	Rolling,
	Parrying,
	Stunned
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	// 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) ECombatState CombatState;

	// 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* RollMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* ParryMontage;

	// 전투 능력치
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float AttackRange = 150.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float AttackDamage = 25.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float RollSpeed = 250.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float RollAvoidTime = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float StunOnDamageTime = 0.3f;

	// 상호작용
	void Attack();
	void Roll();
	void Parry();
	void Damage(float Damage, const FVector& DamageDirection);

private:
	//로직
	FTimerHandle StateTimerHandle;
	void SetCombatState(ECombatState NewState);
	void PerformAttackTrace() const;

	// 델리게이트
	UFUNCTION() void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void EndStun();
};

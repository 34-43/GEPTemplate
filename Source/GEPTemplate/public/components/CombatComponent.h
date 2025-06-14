﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


/**
 * 소유자인 캐릭터의 (전투)상태를 나타내기 위한 열거형.
 * @author 34-43
 */
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	IdleMoving,
	BufferTime,
	Attacking,
	Rolling,
	Parrying,
	Staggered
};

// 피격되거나, 패링하거나, 패링되었을 경우, 소유주에게 알려 전용 처리(주로 소리나 파티클 등 시각적 처리)를 하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamagedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnParriedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaggeredSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * 소유자의 현재 (전투)상태. 소유자 캐릭터에서 필요 시 SetCombatState를 이용하여 직접 지정할 수 있음.
	 * @author 34-43
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)ECombatState CombatState;

	// 애니메이션 몽타주 속성. CDO에서 기본값으로 로드되며, 모든 소유자가 전부 사용하지는 않는다.
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* RollMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* ParryMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* StaggerMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* DeathMontage;

	// 특정 애니메이션 몽타주 리소스의 섹션 이름을 CDO에서 지정하기 위한 속성.
	UPROPERTY(EditDefaultsOnly, Category = "Animation") TArray<FName> AttackMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "Animation") TArray<FName> ParryMontageSections;

	// 전투 능력치
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float AttackRange = 250.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float AttackDamage = 25.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat") float StaggerOnParriedTime = 2.0f;

	// 상호작용
	virtual void Attack();
	virtual void Parry();
	virtual void Damage(int32 Damage, const FVector& DamageDirection, UCombatComponent* Performer);
	virtual void ParrySuccess(UCombatComponent* Performer);
	virtual void PauseMovement();
	virtual void ResumeMovement();
	virtual void SetCombatState(ECombatState NewState);
	virtual void PerformAttackSweep();
	FVector GetLastHitDirection() { return LastHitDirection; }
	float DamageBoost = 1.0f;//캐릭터에서 수정할 수 있음

	// 이벤트
	UPROPERTY(BlueprintAssignable, Category="Event") FOnDamagedSignature OnDamaged;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnParriedSignature OnParried;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnStaggeredSignature OnStaggered;
	
	// 로직
	FTimerHandle StateTimerHandle;
	FVector LastHitDirection;

	// 델리게이트
	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};

#pragma once

#include "CoreMinimal.h"
#include "components/CombatComponent.h"
#include "MeleeCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UMeleeCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:
	UMeleeCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Montage Setting
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MeleeAttackA_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MeleeAttackB_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MeleeAttackC_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MeleeParry_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MeleeParrySuccess_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MeleeDeathA_M; // Montage

	virtual void Attack() override;
	virtual void Parry() override;
	virtual void Damage(int32 Damage, const FVector& DamageDirection, UCombatComponent* Performer) override;
	virtual void ParrySuccess(UCombatComponent* Performer) override;
	virtual void PauseMovement() override;
	virtual void ResumeMovement() override;
	virtual void SetCombatState(ECombatState NewState) override;
	virtual void PerformAttackSweep() override;

	UPROPERTY(BlueprintAssignable, Category="Event") FOnDamagedSignature MeleeOnDamaged;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnParriedSignature MeleeOnParried;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnStaggeredSignature MeleeOnStaggered;

	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
};

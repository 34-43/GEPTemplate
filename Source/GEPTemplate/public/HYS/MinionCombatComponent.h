// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "components/CombatComponent.h"
#include "MinionCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UMinionCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:
	UMinionCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Montage Setting
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MinionAttackA_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MinionAttackB_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MinionAttackC_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MinionParry_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MinionParrySuccess_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MinionDeathA_M; // Montage
	UPROPERTY(VisibleAnywhere, Category = "Animation") UAnimMontage* MinionDeathB_M; // Montage

	virtual void Attack() override;
	virtual void Parry() override;
	virtual void Damage(int32 Damage, const FVector& DamageDirection, UCombatComponent* Performer) override;
	virtual void ParrySuccess(UCombatComponent* Performer) override;
	virtual void PauseMovement() override;
	virtual void ResumeMovement() override;
	virtual void SetCombatState(ECombatState NewState) override;
	virtual void PerformAttackSweep() override;

	UPROPERTY(BlueprintAssignable, Category="Event") FOnDamagedSignature MinionOnDamaged;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnParriedSignature MinionOnParried;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnStaggeredSignature MinionOnStaggered;

	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
};

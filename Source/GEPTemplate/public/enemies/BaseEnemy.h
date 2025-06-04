#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class GEPTEMPLATE_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	UFUNCTION()
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void TickRenderWidget(APlayerController* PC);

	// 컴포넌트
	// UPROPERTY(VisibleAnywhere) class UCombatComponent* CombatC;
	// UPROPERTY(VisibleAnywhere) class UHealthComponent* HealthC;
	// UPROPERTY(VisibleAnywhere) class UBehaviorComponent* BehaviorC;
	UPROPERTY(VisibleAnywhere) class UWidgetComponent* FloatingWidgetC;
	UPROPERTY(VisibleAnywhere) class UFocusedComponent* FocusedC;

	// 팩토리
	UPROPERTY(VisibleAnywhere) UParticleSystem* DamagedFxF;
	UPROPERTY(VisibleAnywhere) UParticleSystem* ParriedFxF;
	UPROPERTY(VisibleAnywhere) USoundBase* DamagedSfxF;
	UPROPERTY(VisibleAnywhere) USoundBase* ParriedSfxF;

	// 능력치
	UPROPERTY(VisibleAnywhere, Category = "AI") float DetectionRadius = 1000.0f;
	UPROPERTY(VisibleAnywhere, Category = "AI") float AttackRange = 200.0f;
	UPROPERTY(VisibleAnywhere, Category = "AI") float LateralDistance = 300.0f;
	UPROPERTY(VisibleAnywhere, Category = "AI") float StaggerDuration = 2.0f;

	// 델리게이트 핸들러
	// UFUNCTION() void HandleDeath();
	// UFUNCTION() void HandleDamaged();
	// UFUNCTION() void HandleParried();
	// UFUNCTION() void HandleStaggered();
	// virtual void HandleDeath();
	// virtual void HandleDamaged();
	// virtual void HandleParried();
	// virtual void HandleStaggered();
	// virtual void RagDollImpulse();

protected:
	// 로직
	FTimerHandle TimerHandle;
	float AttackTimer = 0.0f;
	UPROPERTY() APlayerController* MainPlayerController;
};

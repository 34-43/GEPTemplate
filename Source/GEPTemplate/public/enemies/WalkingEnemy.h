#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "WalkingEnemy.generated.h"

UCLASS()
class GEPTEMPLATE_API AWalkingEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AWalkingEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleAnywhere) class UCombatComponent* CombatC;
	UPROPERTY(VisibleAnywhere) class UHealthComponent* HealthC;
	UPROPERTY(VisibleAnywhere) class UWalkingBehaviorComponent* BehaviorC;
	
	UFUNCTION() void HandleDamaged();
	UFUNCTION() void HandleParried();
	UFUNCTION() void HandleStaggered();
	UFUNCTION() void HandleDeath();

	void RagDollImpulse();
};

#pragma once

#include "CoreMinimal.h"
#include "enemies/BaseEnemy.h"
#include "Melee.generated.h"

UCLASS()
class GEPTEMPLATE_API AMelee : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AMelee();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Component
	UPROPERTY(VisibleAnywhere) USkeletalMeshComponent* MeshC;

	UPROPERTY(VisibleAnywhere) class UMeleeCombatComponent* MeleeCombatC;
	UPROPERTY(VisibleAnywhere) class UHealthComponent* MeleeHealthC;
	UPROPERTY(VisibleAnywhere) class UMeleeBehaviorComponent* MeleeBehaviorC;
	UPROPERTY(VisibleAnywhere) class UCoinGenerator* CoinC;
	
	UFUNCTION() void HandleDamaged();
	UFUNCTION() void HandleParried();
	UFUNCTION() void HandleStaggered();
	UFUNCTION() void HandleDeath();

	void RagDollImpulse();
	
	bool bIgnoreMove = false;
	void SetIgnoreMove(const bool Value) { bIgnoreMove = Value; }
};

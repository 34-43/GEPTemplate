#pragma once

#include "CoreMinimal.h"
#include "enemies/BaseEnemy.h"
#include "Minion.generated.h"

UCLASS()
class GEPTEMPLATE_API AMinion : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AMinion();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Component
	UPROPERTY(VisibleAnywhere) USkeletalMeshComponent* MeshC;

	UPROPERTY(VisibleAnywhere) class UMinionCombatComponent* MinionCombatC;
	UPROPERTY(VisibleAnywhere) class UHealthComponent* MinionHealthC;
	UPROPERTY(VisibleAnywhere) class UMinionBehaviorComponent* MinionBehaviorC;
	UPROPERTY(VisibleAnywhere) class UCoinGenerator* CoinC;
	
	UFUNCTION() void HandleDamaged();
	UFUNCTION() void HandleParried();
	UFUNCTION() void HandleStaggered();
	UFUNCTION() void HandleDeath();

	void RagDollImpulse();
	
	bool bIgnoreMove = false;
	void SetIgnoreMove(const bool Value) { bIgnoreMove = Value; }

};

#pragma once

#include "CoreMinimal.h"
#include "MinionCombatComponent.h"
#include "components/BehaviorComponent.h"
#include "MinionBehaviorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UMinionBehaviorComponent : public UBehaviorComponent
{
	GENERATED_BODY()

public:
	UMinionBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	UMinionCombatComponent* MyCombatC;

	virtual void TickBattle() override;
	void PerformAttack();
	void OnAttackEnd();
	FTimerHandle AttackEndTimerHandle;

private:
	bool bAttackFlag = true;
};

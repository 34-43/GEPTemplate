#pragma once

#include "CoreMinimal.h"
#include "MeleeCombatComponent.h"
#include "components/BehaviorComponent.h"
#include "MeleeBehaviorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UMeleeBehaviorComponent : public UBehaviorComponent
{
	GENERATED_BODY()

public:
	UMeleeBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	UMeleeCombatComponent* MyCombatC;

	virtual void TickBattle() override;
	void PerformAttack();
	void OnAttackEnd();
	FTimerHandle AttackEndTimerHandle;

private:
	bool bAttackFlag = true;
};

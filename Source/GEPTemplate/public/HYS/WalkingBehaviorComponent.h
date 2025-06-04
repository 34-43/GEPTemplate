#pragma once

#include "CoreMinimal.h"
#include "components/BehaviorComponent.h"
#include "WalkingBehaviorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UWalkingBehaviorComponent : public UBehaviorComponent
{
	GENERATED_BODY()

public:
	UWalkingBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY()	UCombatComponent* MyCombatC;
	
	virtual void TickBattle() override;
	void PerformComboAttack();
};

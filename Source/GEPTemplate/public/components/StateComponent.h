#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"

UENUM(Blueprintable)
enum class EEnemyState:uint8
{
	Idle,
	Move,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStateComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	EEnemyState mState = EEnemyState::Idle;

	void IdleState();
	void MoveState();

	float idleDelayTime = 2;
	float currentTime;

	UPROPERTY()
	class AMainCharacter* target;

	UPROPERTY()
	class ABaseEnemy* me;
};

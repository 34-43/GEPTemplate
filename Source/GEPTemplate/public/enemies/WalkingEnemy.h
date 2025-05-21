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
	virtual void TickAI(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Component
	UPROPERTY(VisibleAnywhere) class UStateComponent* StateC;
};

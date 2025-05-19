#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnStaminaChangedSignature,
	float, NewStamina,
	float, MaxStamina
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// 스테이터스
	UPROPERTY(EditDefaultsOnly, Category="Stamina") float MaxStamina = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category="Stamina") float CurrentStamina;

	// Event
	UPROPERTY(BlueprintAssignable, Category="Events") FOnStaminaChangedSignature OnStaminaChanged;

	UFUNCTION()
	void UpdateStamina(float Delta);
};

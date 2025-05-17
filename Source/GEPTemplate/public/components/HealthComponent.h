#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealthChangedSignature,
	int32, NewHealth,
	int32, MaxHealth
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 스테이터스
	UPROPERTY(EditDefaultsOnly, Category = "Health") int32 MaxHealth = 100.0;
	UPROPERTY(EditDefaultsOnly, Category = "Health") int32 CurrentHealth;

	// 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Events") FOnHealthChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events") FOnDeathSignature OnDeath;

	UFUNCTION()
	void UpdateHealth(int32 Delta);
};

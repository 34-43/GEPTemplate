#include "components/HealthComponent.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UHealthComponent::UpdateHealth(const int32 Delta)
{
	if (CurrentHealth == 0) return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Delta, 0, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0)
	{
		OnDeath.Broadcast();
	}
}
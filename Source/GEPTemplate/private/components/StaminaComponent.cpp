#include "components/StaminaComponent.h"


UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}

void UStaminaComponent::UpdateStamina(float Delta)
{
	// change current stamina
	CurrentStamina = FMath::Clamp(CurrentStamina + Delta, 0.0f, MaxStamina);

	// alert everyone about update
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

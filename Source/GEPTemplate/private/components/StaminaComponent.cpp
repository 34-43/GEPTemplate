#include "components/StaminaComponent.h"


UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}


void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UStaminaComponent::UpdateStamina(float Delta)
{
	// change current stamina
	CurrentStamina = FMath::Clamp(CurrentStamina + Delta, 0.0f, MaxStamina);

	// alert everyone about update
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	
}

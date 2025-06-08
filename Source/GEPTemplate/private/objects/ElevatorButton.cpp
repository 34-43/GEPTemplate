// WaterDispenser.cpp

#include "objects/ElevatorButton.h"
#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "allies/MainCharacter.h"
#include "TimerManager.h"

AElevatorButton::AElevatorButton()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("Object"));
	RootComponent = Mesh;

	// 상호작용 컴포넌트 설정
	InteractC = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void AElevatorButton::Interact(AActor* Caller)
{
	InteractC->SetPower(false);
	OnInteract.Broadcast();
	// 몇 초 후 다시 활성화
	GetWorld()->GetTimerManager().SetTimer(
		ReenableTimerHandle, this, &AElevatorButton::ReenablePower, 0.4f, false
	);
}

void AElevatorButton::ReenablePower()
{
	if (InteractC)
	{
		InteractC->SetPower(true);
	}
}
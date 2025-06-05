// WaterDispenser.cpp

#include "objects/WaterDispenser.h"
#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "allies/MainCharacter.h"
#include "components/StaminaComponent.h"
#include "TimerManager.h"

AWaterDispenser::AWaterDispenser()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("Object"));
	RootComponent = Mesh;

	// 상호작용 컴포넌트 설정
	InteractC = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void AWaterDispenser::BeginPlay()
{
	Super::BeginPlay();
}

void AWaterDispenser::Interact(AActor* Caller)
{
	// UE_LOG(LogTemp, Log, TEXT("%s interacted with %s!"), *Caller->GetName(), *GetName());
	if (!Caller) return;
	
	AMainCharacter* Player = Cast<AMainCharacter>(Caller);
	if (!Player) return;
	
	UStaminaComponent* StaminaComp = Player->FindComponentByClass<UStaminaComponent>();
	if (!StaminaComp) return;
	StaminaComp->UpdateStamina(50);

	InteractC->SetPower(false);
	// 몇 초 후 다시 활성화
	GetWorld()->GetTimerManager().SetTimer(
		ReenableTimerHandle, this, &AWaterDispenser::ReenablePower, 2.0f, false
	);
}

void AWaterDispenser::ReenablePower()
{
	if (InteractC)
	{
		InteractC->SetPower(true);
	}
}
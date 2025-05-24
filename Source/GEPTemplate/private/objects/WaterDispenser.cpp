// WaterDispenser.cpp

#include "objects/WaterDispenser.h"
#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

AWaterDispenser::AWaterDispenser()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
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
	if (!Caller) return;
	UE_LOG(LogTemp, Log, TEXT("%s interacted with %s!"), *Caller->GetName(), *GetName());
}

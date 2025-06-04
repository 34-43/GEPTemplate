// SavePoint.cpp

#include "objects/SavePoint.h"
#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundAttenuation.h"
#include "allies/MainCharacter.h"
#include "systems/GameSettingsInstance.h"
#include "TimerManager.h"

// Sets default values
ASavePoint::ASavePoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	// 물체는 통과 가능 & 상호작용 가능한 프로파일
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = Mesh;

	// 상호작용 컴포넌트 설정
	InteractC = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractC->InteractionDuration = 2.5f;
	InteractC->InteractRange = 200.f;
}

void ASavePoint::BeginPlay()
{
	Super::BeginPlay();
}

void ASavePoint::Interact(AActor* Caller)
{
	if (!Caller) return;

	AMainCharacter* Player = Cast<AMainCharacter>(Caller);
	if (!Player) return;
	
	UGameSettingsInstance* GI = Cast<UGameSettingsInstance>(GetGameInstance());
	if (GI)
	{
		GI->SavePlayerData(Player);
	}
	
	UE_LOG(LogTemp, Log, TEXT("SavePoint '%s' saved data for player '%s'."), *GetName(), *Player->GetName());
	InteractC->SetPower(false);
	// 몇 초 후 다시 활성화
	GetWorld()->GetTimerManager().SetTimer(
		ReenableTimerHandle, this, &ASavePoint::ReenablePower, 2.0f, false
	);
}

void ASavePoint::ReenablePower()
{
	if (InteractC)
	{
		InteractC->SetPower(true);
	}
}
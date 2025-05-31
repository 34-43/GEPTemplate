// SavePoint.cpp

#include "objects/SavePoint.h"
#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"
#include "allies/MainCharacter.h"
#include "systems/GameSettingsInstance.h"

// Sets default values
ASavePoint::ASavePoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	// 물체는 통과 가능 & 상호작용 가능한 프로파일
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = Mesh;

	// 상호작용 컴포넌트 설정
	InteractC = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractC->InteractionDuration = 2.5f;
	InteractC->InteractRange = 200.f;

	// 오디오 컴포넌트 설정
	AudioC = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioC->SetupAttachment(RootComponent);
	AudioC->bAutoActivate = false;
	SoundRange = 800.f;
}

void ASavePoint::BeginPlay()
{
	Super::BeginPlay();
}

void ASavePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (InteractC->GetDistanceToPlayer() <= SoundRange)
	{
		if (!AudioC->IsPlaying() && SaveSound)
		{
			AudioC->SetSound(SaveSound);
			AudioC->Play();
		}
	}
	else
	{
		if (AudioC->IsPlaying())
		{
			AudioC->Stop();
		}
	}
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
}

void ASavePoint::PlaySound()
{
	if (SaveSound && AudioC)
	{
		AudioC->SetSound(SaveSound);
		AudioC->Play();
	}
}

void ASavePoint::StopSound()
{
	if (AudioC->IsPlaying())
	{
		AudioC->Stop();
	}
}
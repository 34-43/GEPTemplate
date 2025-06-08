// SavePoint.cpp

#include "objects/SavePoint.h"

#include "GEPTemplate.h"
#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundAttenuation.h"
#include "allies/MainCharacter.h"
#include "systems/GameSettingsInstance.h"
#include "TimerManager.h"
#include "allies/GameAlertUIWidget.h"

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

	// UI 클래스 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> GameAlert(TEXT("/Game/UI/WBP_GameAlertUI.WBP_GameAlertUI_C"));
	if (GameAlert.Succeeded()) { GameAlertUI_W = GameAlert.Class; }
}

void ASavePoint::BeginPlay()
{
	Super::BeginPlay();
	InitializeGameAlert();
}

void ASavePoint::InitializeGameAlert()
{
	if (GameAlertUI_W)
	{
		GameAlertUIWidget = CreateWidget<UUserWidget>(GetWorld(), GameAlertUI_W);
		if (GameAlertUIWidget)
		{
			GameAlertUIWidget->AddToViewport();
			GameAlertUIWidget->SetVisibility(ESlateVisibility::Hidden); // 처음엔 숨김
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
	InteractC->SetPower(false);

	// UI 띄우기
	ShowSavedUI();
	
	// 몇 초 후 다시 활성화
	GetWorld()->GetTimerManager().SetTimer(
		ReenableTimerHandle, this, &ASavePoint::ReenablePower, 2.0f, false
	);
}

void ASavePoint::ShowSavedUI()
{
	if (auto AlertUI = Cast<UGameAlertUIWidget>(GameAlertUIWidget))
	{
		AlertUI->SetVisibility(ESlateVisibility::Visible);
		AlertUI->PlayAlert(TEXT("PROGRESS SAVED"), FLinearColor::Green,2);
	}
}

void ASavePoint::ReenablePower()
{
	if (InteractC)
	{
		InteractC->SetPower(true);
	}
}
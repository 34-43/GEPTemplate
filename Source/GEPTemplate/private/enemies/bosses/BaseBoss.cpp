#include "enemies/bosses/BaseBoss.h"

#include "allies/GameAlertUIWidget.h"
#include "allies/MainCharacter.h"
#include "components/FocusedComponent.h"
#include "components/HealthComponent.h"
#include "enemies/bosses/BossHUDWidget.h"


ABaseBoss::ABaseBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 세팅
	StaticMeshC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshC->SetupAttachment(GetMesh());
	TotalHealthC = CreateDefaultSubobject<UHealthComponent>(TEXT("TotalHealthComponent"));
	FocusedC = CreateDefaultSubobject<UFocusedComponent>(TEXT("FocusedComponent"));

	// 위젯 클래스 로드
	static ConstructorHelpers::FClassFinder<UUserWidget> BossHUD(TEXT("/Game/UI/WBP_BossHUD.WBP_BossHUD_C"));
	if (BossHUD.Succeeded()) { BossHUD_W = BossHUD.Class; }
	static ConstructorHelpers::FClassFinder<UUserWidget> GameAlertUI(TEXT("/Game/UI/WBP_GameAlertUI.WBP_GameAlertUI_C"));
	if (GameAlertUI.Succeeded()) { GameAlertUI_W = GameAlertUI.Class; }
}

void ABaseBoss::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void ABaseBoss::TriggerBossFight()
{
	InitializeBossHUD();
	InitializeGameAlert();
}

void ABaseBoss::InitializeBossHUD()
{
	if (BossHUD_W)
	{
		BossHUDWidget = CreateWidget<UUserWidget>(GetWorld(), BossHUD_W);
		if (BossHUDWidget)
		{
			BossHUDWidget->AddToViewport();

			// 처음 상태 반영
			if (UBossHUDWidget* BossHUD = Cast<UBossHUDWidget>(BossHUDWidget))
			{
				BossHUD->HandleHealthChanged(TotalHealthC->CurrentHealth, TotalHealthC->MaxHealth);
			}
		}
	}
}

void ABaseBoss::InitializeGameAlert()
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

void ABaseBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBoss::ShowVictoryUI() const
{
	if (UGameAlertUIWidget* AlertUI = Cast<UGameAlertUIWidget>(GameAlertUIWidget))
	{
		AlertUI->SetVisibility(ESlateVisibility::Visible);
		AlertUI->PlayAlert(TEXT("LOTUS REVIVED"), FLinearColor::Green);
	}
}

void ABaseBoss::HandleDeath()
{
}

void ABaseBoss::HandleDamaged()
{
}

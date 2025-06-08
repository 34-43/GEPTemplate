#include "enemies/bosses/BaseBoss.h"

#include "allies/GameAlertUIWidget.h"
#include "allies/MainCharacter.h"
#include "components/FocusedComponent.h"
#include "components/HealthComponent.h"
#include "enemies/bosses/BossHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "systems/CreditRoll.h"
#include "systems/MenuGameMode.h"


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
	static ConstructorHelpers::FClassFinder<UUserWidget>
		GameAlertUI(TEXT("/Game/UI/WBP_GameAlertUI.WBP_GameAlertUI_C"));
	if (GameAlertUI.Succeeded()) { GameAlertUI_W = GameAlertUI.Class; }
	static ConstructorHelpers::FClassFinder<UUserWidget> CreditRollUI(
		TEXT("/Game/UI/WBP_CreditRoll.WBP_CreditRoll_C"));
	if (CreditRollUI.Succeeded()) { CreditRoll_W = CreditRollUI.Class; }

	// 팩토리 로드
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEffect(
		TEXT("/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect"));
	if (ExplosionEffect.Succeeded()) { ExplosionFxF = ExplosionEffect.Object; }
}

void ABaseBoss::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	TotalHealthC->OnDeath.AddDynamic(this, &ABaseBoss::HandleDeath);
}

void ABaseBoss::TriggerBossFight()
{
	InitializeBossHUD();
	InitializeGameAlert();
	InitializeCreditRoll();
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
				TotalHealthC->OnHealthChanged.AddDynamic(BossHUD, &UBossHUDWidget::HandleHealthChanged);
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

void ABaseBoss::InitializeCreditRoll()
{
	if (CreditRoll_W)
	{
		CreditRollWidget = CreateWidget<UUserWidget>(GetWorld(), CreditRoll_W);
		if (CreditRollWidget)
		{
			CreditRollWidget->AddToViewport();
			CreditRollWidget->SetVisibility(ESlateVisibility::Hidden);
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
		AlertUI->PlayAlert(TEXT("MISSION COMPLETE"), FLinearColor::Yellow);
	}
}

void ABaseBoss::HandleDeath()
{
	ShowVictoryUI();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFxF, GetActorLocation(), FRotator::ZeroRotator,
	                                         FVector(100.0f));
	
	// 메인메뉴로 이동
	GetWorldTimerManager().SetTimer(BaseBossTimerHandle, [this]()
	{
		CreditRollWidget->SetVisibility(ESlateVisibility::Visible);
		if (auto AB = Cast<UCreditRoll>(CreditRollWidget)) AB->PlayAnimation(AB->Roll);
		GetWorldTimerManager().SetTimer(BaseBossTimerHandle, [this]()
		{
			FName MenuLevelName = TEXT("MenuMap");
			FString GameModePath = AMenuGameMode::StaticClass()->GetPathName(); 
			// 예시 반환값: "/Script/YourProjectName.MenuGameMode"

			FString Options = FString::Printf(TEXT("Game=%s"), *GameModePath);
			UGameplayStatics::OpenLevel(GetWorld(), FName("MenuMap"), true, *Options);
		}, 5.0f, false);
	}, 4.0f, false);
}

void ABaseBoss::HandleDamaged()
{
}

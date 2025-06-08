#include "HYS/Minion.h"

#include "Components/CapsuleComponent.h"
#include "components/CombatComponent.h"
#include "components/FocusedComponent.h"
#include "components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "enemies/EnemyFloatingWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYS/MinionBehaviorComponent.h"
#include "HYS/MinionCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "components/CoinGenerator.h"

AMinion::AMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	GetCharacterMovement()->bOrientRotationToMovement = true; // 메시를 이동방향으로 계속 갱신하는 기능 사용

	// 부유 위젯 컴포넌트 설정
	FloatingWidgetC = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatingWidgetComponent"));
	FloatingWidgetC->SetDrawSize(FVector2D(100, 8));
	FloatingWidgetC->SetRelativeLocation(FVector(0, 0, 130));
	FloatingWidgetC->SetWidgetSpace(EWidgetSpace::World);
	FloatingWidgetC->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FClassFinder<UUserWidget> FloatingWidgetBP(
		TEXT("/Game/UI/WBP_EnemyFloating.WBP_EnemyFloating_C"));
	if (FloatingWidgetBP.Succeeded()) { FloatingWidgetC->SetWidgetClass(FloatingWidgetBP.Class); }

	MinionCombatC = CreateDefaultSubobject<UMinionCombatComponent>(TEXT("MinionCombatComponent"));

	// 체력 컴포넌트 설정
	MinionHealthC = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	MeshC = GetMesh();
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // Z축 위치 -90
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Yaw -90도 회전

	// 애니메이션 BP 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(
		TEXT("/Game/HYS/Minion/ABP_Minion.ABP_Minion_C"));
	if (AnimBP.Succeeded()) { MeshC->SetAnimInstanceClass(AnimBP.Class); }

	// 피집중 컴포넌트 설정
	FocusedC = CreateDefaultSubobject<UFocusedComponent>(TEXT("FocusedComponent"));
	FocusedC->SetupWidgetAttachment(MeshC, TEXT("focus"));

	MinionBehaviorC = CreateDefaultSubobject<UMinionBehaviorComponent>(TEXT("MinionBehaviorComponent"));

	// 코인 드랍 컴포넌트 설정
	CoinC = CreateDefaultSubobject<UCoinGenerator>(TEXT("CoinGenerator"));
	CoinC->SetupAttachment(MeshC);
}

void AMinion::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("HealthC is valid? %s"), MinionHealthC ? TEXT("Yes") : TEXT("NO!"));

	// UI 렌더링에 필요한 1P 컨트롤러 지정
	MainPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// 체력 컴포넌트의 두 델리게이트에 각각 1.체력 UI 업데이트 처리, 2. 사망 처리 핸들러 연결
	if (const auto Widget = Cast<UEnemyFloatingWidget>(FloatingWidgetC->GetWidget()))
	{
		MinionHealthC->OnHealthChanged.AddDynamic(Widget, &UEnemyFloatingWidget::HandleHealthChanged);
	}
	MinionHealthC->OnDeath.AddDynamic(this, &AMinion::HandleDeath);

	MinionCombatC->MinionOnDamaged.AddDynamic(this, &AMinion::HandleDamaged);
	MinionCombatC->MinionOnParried.AddDynamic(this, &AMinion::HandleParried);
	MinionCombatC->MinionOnStaggered.AddDynamic(this, &AMinion::HandleStaggered);
}

void AMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMinion::HandleDamaged()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedFxF, GetActorLocation());
	UGameplayStatics::SpawnSound2D(GetWorld(), DamagedSfxF);
}

void AMinion::HandleParried()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParriedFxF,
		                                         GetMesh()->GetSocketLocation(TEXT("hand_l_socket")));
		UGameplayStatics::SpawnSound2D(GetWorld(), ParriedSfxF);
	}, 0.23f, false);
}

void AMinion::HandleStaggered()
{
}

void AMinion::HandleDeath()
{
	// 틱 비활성화
	SetActorTickEnabled(false);

	// UI 삭제
	if (FloatingWidgetC)
	{
		FloatingWidgetC->DestroyComponent();
		FloatingWidgetC = nullptr;
	}
	if (FocusedC)
	{
		FocusedC->DestroyComponent();
		FocusedC = nullptr;
	}

	// 아이템 드랍
	CoinC->GenerateCoins(1);

	// 래그돌 전환 및 넉
	RagDollImpulse();

	// 삭제 시간 설정
	SetLifeSpan(5.0f);
}

void AMinion::RagDollImpulse()
{
	// 메시 가져오기
	if (!MeshC) MeshC = GetMesh(); // 필요시 갱신

	// 컨트롤러 해제
	DetachFromControllerPendingDestroy();

	// 이동, 충돌 비활성화
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 죽음 애니메이션 재생 시도
	if (MinionCombatC && MeshC)
	{
		if (UAnimInstance* AnimInst = MeshC->GetAnimInstance())
		{
			// 실행 중인 몽타주 종료
			AnimInst->Montage_Stop(0.1f); // 0.1초 블렌딩으로 자연스럽게 끊기

			// 50% 확률로 둘 중 하나의 죽음 애니메이션 실행
			UAnimMontage* DeathMontage = FMath::RandBool()
				                             ? MinionCombatC->MinionDeathA_M
				                             : MinionCombatC->MinionDeathB_M;

			if (DeathMontage)
			{
				AnimInst->Montage_Play(DeathMontage);
			}
		}
	}
}

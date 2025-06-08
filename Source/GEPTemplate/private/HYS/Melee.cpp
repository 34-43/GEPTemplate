#include "HYS/Melee.h"

#include "Components/CapsuleComponent.h"
#include "components/FocusedComponent.h"
#include "components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "enemies/EnemyFloatingWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYS/MeleeBehaviorComponent.h"
#include "HYS/MeleeCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "components/CoinGenerator.h"


AMelee::AMelee()
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

	MeleeCombatC = CreateDefaultSubobject<UMeleeCombatComponent>(TEXT("MeleeCombatComponent"));

	// 체력 컴포넌트 설정
	MeleeHealthC = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	MeshC = GetMesh();
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // Z축 위치 -90
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Yaw -90도 회전

	// 애니메이션 BP 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(
		TEXT("/Game/HYS/Melee/ABP_Melee.ABP_Melee_C"));
	if (AnimBP.Succeeded()) { MeshC->SetAnimInstanceClass(AnimBP.Class); }

	// 피집중 컴포넌트 설정
	FocusedC = CreateDefaultSubobject<UFocusedComponent>(TEXT("FocusedComponent"));
	FocusedC->SetupWidgetAttachment(MeshC, TEXT("focus"));

	MeleeBehaviorC = CreateDefaultSubobject<UMeleeBehaviorComponent>(TEXT("MeleeBehaviorComponent"));
	
	// 코인 드랍 컴포넌트 설정
	CoinC = CreateDefaultSubobject<UCoinGenerator>(TEXT("CoinGenerator"));
	CoinC->SetupAttachment(MeshC);
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("HealthC is valid? %s"), MeleeHealthC ? TEXT("Yes") : TEXT("NO!"));

	// UI 렌더링에 필요한 1P 컨트롤러 지정
	MainPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// 체력 컴포넌트의 두 델리게이트에 각각 1.체력 UI 업데이트 처리, 2. 사망 처리 핸들러 연결
	if (const auto Widget = Cast<UEnemyFloatingWidget>(FloatingWidgetC->GetWidget()))
	{
		MeleeHealthC->OnHealthChanged.AddDynamic(Widget, &UEnemyFloatingWidget::HandleHealthChanged);
	}
	MeleeHealthC->OnDeath.AddDynamic(this, &AMelee::HandleDeath);

	MeleeCombatC->MeleeOnDamaged.AddDynamic(this, &AMelee::HandleDamaged);
	MeleeCombatC->MeleeOnParried.AddDynamic(this, &AMelee::HandleParried);
	MeleeCombatC->MeleeOnStaggered.AddDynamic(this, &AMelee::HandleStaggered);
}

void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMelee::HandleDamaged()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedFxF, GetActorLocation());
	UGameplayStatics::SpawnSound2D(GetWorld(), DamagedSfxF);
}

void AMelee::HandleParried()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParriedFxF,
		                                         GetMesh()->GetSocketLocation(TEXT("hand_l_socket")));
		UGameplayStatics::SpawnSound2D(GetWorld(), ParriedSfxF);
	}, 0.23f, false);
}

void AMelee::HandleStaggered()
{
}

void AMelee::HandleDeath()
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
	CoinC->GenerateCoins(2);

	// 래그돌 전환 및 넉
	RagDollImpulse();

	// 삭제 시간 설정
	SetLifeSpan(5.0f);
}

void AMelee::RagDollImpulse()
{
	// 메시 가져오기
	if (!MeshC) MeshC = GetMesh(); // 필요시 갱신

	// 컨트롤러 해제
	DetachFromControllerPendingDestroy();

	// 이동, 충돌 비활성화
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 죽음 애니메이션 재생 시도
	if (MeleeCombatC && MeshC)
	{
		if (UAnimInstance* AnimInst = MeshC->GetAnimInstance())
		{
			// 실행 중인 몽타주 종료
			AnimInst->Montage_Stop(0.1f); // 0.1초 블렌딩으로 자연스럽게 끊기
			// 죽음 애니메이션 실행
			UAnimMontage* DeathMontage = MeleeCombatC->MeleeDeathA_M;

			if (DeathMontage)
			{
				AnimInst->Montage_Play(DeathMontage);
			}
		}
	}
}

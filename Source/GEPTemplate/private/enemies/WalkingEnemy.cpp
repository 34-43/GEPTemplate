#include "enemies/WalkingEnemy.h"

#include "components/CombatComponent.h"
#include "components/CoinGenerator.h"
#include "components/BehaviorComponent.h"
#include "Components/CapsuleComponent.h"
#include "components/FocusedComponent.h"
#include "components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "enemies/EnemyFloatingWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYS/WalkingBehaviorComponent.h"
#include "Kismet/GameplayStatics.h"


AWalkingEnemy::AWalkingEnemy()
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
	
	CombatC = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> WalkingEnemyAttackMontageFinder(
		TEXT("/Game/Features/Mannequin/Animations/Montage/WalkingEnemyAttackMontage.WalkingEnemyAttackMontage"));
	if (WalkingEnemyAttackMontageFinder.Succeeded()) { CombatC->AttackMontage = WalkingEnemyAttackMontageFinder.Object; }
	
	// 체력 컴포넌트 설정
	HealthC = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// 기본 메시 포인터 로드
	USkeletalMeshComponent* MeshC = GetMesh();
	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT("/Game/Features/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin"));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // Z축 위치 -90
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Yaw -90도 회전

	// 애니메이션 BP 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(
		TEXT("/Game/Blueprints/ABP_MainCharacter.ABP_MainCharacter_C"));
	if (AnimBP.Succeeded()) { MeshC->SetAnimInstanceClass(AnimBP.Class); }

	// 피집중 컴포넌트 설정
	FocusedC = CreateDefaultSubobject<UFocusedComponent>(TEXT("FocusedComponent"));
	FocusedC->SetupWidgetAttachment(MeshC, TEXT("focus"));

	// Set State Component
	WalkingBehaviorC = CreateDefaultSubobject<UWalkingBehaviorComponent>(TEXT("BehaviorComponent"));
	
	// 코인 드랍 컴포넌트 설정
	CoinC = CreateDefaultSubobject<UCoinGenerator>(TEXT("CoinGenerator"));
	CoinC->SetupAttachment(MeshC);
}

void AWalkingEnemy::BeginPlay()
{
	Super::BeginPlay();

	// UI 렌더링에 필요한 1P 컨트롤러 지정
	MainPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// 체력 컴포넌트의 두 델리게이트에 각각 1.체력 UI 업데이트 처리, 2. 사망 처리 핸들러 연결
	if (const auto Widget = Cast<UEnemyFloatingWidget>(FloatingWidgetC->GetWidget()))
	{
		HealthC->OnHealthChanged.AddDynamic(Widget, &UEnemyFloatingWidget::HandleHealthChanged);
	}
	HealthC->OnDeath.AddDynamic(this, &AWalkingEnemy::HandleDeath);

	CombatC->OnDamaged.AddDynamic(this, &AWalkingEnemy::HandleDamaged);
	CombatC->OnParried.AddDynamic(this, &AWalkingEnemy::HandleParried);
	CombatC->OnStaggered.AddDynamic(this, &AWalkingEnemy::HandleStaggered);
}

void AWalkingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// void AWalkingEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);
// }



void AWalkingEnemy::HandleDamaged()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedFxF, GetActorLocation());
	UGameplayStatics::SpawnSound2D(GetWorld(), DamagedSfxF);
}

void AWalkingEnemy::HandleParried()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParriedFxF,
		                                         GetMesh()->GetSocketLocation(TEXT("hand_l_socket")));
		UGameplayStatics::SpawnSound2D(GetWorld(), ParriedSfxF);
	}, 0.23f, false);
}

void AWalkingEnemy::HandleStaggered()
{
}

void AWalkingEnemy::HandleDeath()
{
	// 틱 비활성화
	if (WalkingBehaviorC)
	{
		WalkingBehaviorC->SetState(EEnemyState::Dead);
	}
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

void AWalkingEnemy::RagDollImpulse()
{
	// 메시 가져오기
	USkeletalMeshComponent* MeshC = GetMesh();

	// 컨트롤러 해제
	DetachFromControllerPendingDestroy();

	// 이동, 충돌 비활성화
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 피직스 설정
	MeshC->SetSimulatePhysics(true);
	MeshC->SetCollisionProfileName(TEXT("Ragdoll"));

	// 날려보내기
	FVector ImpulseVector = -CombatC->GetLastHitDirection();
	GetWorld()->GetTimerManager().SetTimerForNextTick([this, ImpulseVector, MeshC]()
	{
		MeshC->AddImpulse(ImpulseVector, NAME_None, true);
	});
}

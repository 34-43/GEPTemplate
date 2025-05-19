#include "enemies/BaseEnemy.h"

#include "Components/CapsuleComponent.h"
#include "components/CombatComponent.h"
#include "components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "enemies/EnemyFloatingWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// 기본 메시 포인터 로드
	USkeletalMeshComponent* MeshC = GetMesh();

	// 충돌 설정
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");

	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT("/Game/Features/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin"));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }

	// 애니메이션 BP 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(
		TEXT("/Game/Blueprints/ABP_MainCharacter.ABP_MainCharacter_C"));
	if (AnimBP.Succeeded()) { MeshC->SetAnimInstanceClass(AnimBP.Class); }

	// 전투 컴포넌트 설정
	CombatC = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	// 체력 컴포넌트 설정
	HealthC = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// 부유 위젯 컴포넌트 설정
	FloatingWidgetC = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatingWidgetComponent"));
	FloatingWidgetC->SetDrawSize(FVector2D(100, 8));
	FloatingWidgetC->SetRelativeLocation(FVector(0, 0, 130));
	FloatingWidgetC->SetWidgetSpace(EWidgetSpace::World);
	FloatingWidgetC->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FClassFinder<UUserWidget> FloatingWidgetBP(
		TEXT("/Game/UI/WBP_EnemyFloating.WBP_EnemyFloating_C"));
	if (FloatingWidgetBP.Succeeded()) { FloatingWidgetC->SetWidgetClass(FloatingWidgetBP.Class); }

	//초점 위젯 컴포넌트 설정
	FocusingWidgetC = CreateDefaultSubobject<UWidgetComponent>(TEXT("FocusingWidgetComponent"));
	FocusingWidgetC->SetDrawSize(FVector2D(30, 30));
	FocusingWidgetC->SetupAttachment(MeshC, TEXT("focus"));
	FocusingWidgetC->SetRelativeLocation(FVector::ZeroVector);
	FocusingWidgetC->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> FocusingWidgetBP(
		TEXT("/Game/UI/WBP_Focusing.WBP_Focusing_C"));
	if (FocusingWidgetBP.Succeeded()) { FocusingWidgetC->SetWidgetClass(FocusingWidgetBP.Class); }

	// 팩토리 설정
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DamagedEffect(
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DamagedEffect.Succeeded()) { DamagedFxF = DamagedEffect.Object; }
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParriedEffect(
	TEXT("/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect"));
	if (ParriedEffect.Succeeded()) { ParriedFxF = ParriedEffect.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> DamagedSound(
		TEXT("/Game/Features/BaseBallBatSound/bat_machine.bat_machine"));
	if (DamagedSound.Succeeded()) { DamagedSfxF = DamagedSound.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> ParriedSound(
	TEXT("/Game/Features/BaseBallBatSound/bat_metal.bat_metal"));
	if (ParriedSound.Succeeded()) { ParriedSfxF = ParriedSound.Object; }
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	// UI 렌더링에 필요한 1P 컨트롤러 지정
	MainPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// 체력 컴포넌트의 두 델리게이트에 각각 1.체력 UI 업데이트 처리, 2. 사망 처리 핸들러 연결
	if (auto Widget = Cast<UEnemyFloatingWidget>(FloatingWidgetC->GetWidget()))
	{
		HealthC->OnHealthChanged.AddDynamic(Widget, &UEnemyFloatingWidget::HandleHealthChanged);
	}
	HealthC->OnDeath.AddDynamic(this, &ABaseEnemy::HandleDeath);

	// 전투 컴포넌트의 델리게이트들을 토대로 시각적 처리
	CombatC->OnDamaged.AddDynamic(this, &ABaseEnemy::HandleDamaged);
	CombatC->OnParried.AddDynamic(this, &ABaseEnemy::HandleParried);
	CombatC->OnStaggered.AddDynamic(this, &ABaseEnemy::HandleStaggered);
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickRenderWidget(MainPlayerController);

	if (AttackTimer <= 0.0f)
	{
		CombatC->Attack();
		AttackTimer = 5.0f;
	}
	else
	{
		AttackTimer -= DeltaTime;
	}
}

void ABaseEnemy::TickRenderWidget(APlayerController* PC)
{
	if (!FloatingWidgetC || !PC || !PC->PlayerCameraManager) return;
	const FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();

	const float Distance = FVector::DistSquared(CameraLocation, GetActorLocation());

	constexpr float FadeStart = 800.0f * 800.0f;
	constexpr float FadeEnd = 1200.0f * 1200.0f;
	const float OpacityFactor = 1.0f - FMath::GetMappedRangeValueClamped(
		FVector2D(FadeStart, FadeEnd), FVector2D(0.0f, 1.0f),
		Distance);

	if (UUserWidget* Widget = FloatingWidgetC->GetUserWidgetObject())
	{
		Widget->SetRenderOpacity(OpacityFactor);
	}

	// LookAt 회전 구하기
	const FVector WidgetLoc = FloatingWidgetC->GetComponentLocation();
	const FRotator LookAtRot = (CameraLocation - WidgetLoc).Rotation();

	// 위젯 컴포넌트에 적용
	FloatingWidgetC->SetWorldRotation(LookAtRot);
}

void ABaseEnemy::HandleDamaged()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedFxF, GetActorLocation());
	UGameplayStatics::SpawnSound2D(GetWorld(), DamagedSfxF);
}

void ABaseEnemy::HandleParried()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParriedFxF, GetMesh()->GetSocketLocation(TEXT("hand_l_socket")));
		UGameplayStatics::SpawnSound2D(GetWorld(), ParriedSfxF);
	}, 0.23f, false);
}

void ABaseEnemy::HandleStaggered()
{
}


void ABaseEnemy::HandleDeath()
{
	// 틱 비활성화
	SetActorTickEnabled(false);

	// UI 삭제
	if (FloatingWidgetC)
	{
		FloatingWidgetC->DestroyComponent();
		FloatingWidgetC = nullptr;
	}

	// 아이템 드랍

	// 래그돌 전환 및 넉
	RagDollImpulse();

	// 삭제 시간 설정
	SetLifeSpan(5.0f);
}

void ABaseEnemy::RagDollImpulse()
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

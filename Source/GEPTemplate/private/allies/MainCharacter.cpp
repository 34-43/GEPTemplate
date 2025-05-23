#include "allies/MainCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "allies/BaseBullet.h"
#include "allies/GameAlertUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "allies/MinimapCaptureActor.h"       // AMinimapCaptureActor 헤더 포함
#include "allies/PlayerHUDWidget.h"
#include "components/FocusingComponent.h"
#include "components/HealthComponent.h"
#include "components/StaminaComponent.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"


AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 기본 메시 포인터 로드
	USkeletalMeshComponent* MeshC = GetMesh();

	// 충돌 설정
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Character"));

	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT(
			"/Game/Features/Mannequin/Character/rp_manuel_rigged_001_ue4/rp_manuel_rigged_001_ue4.rp_manuel_rigged_001_ue4"));
	MeshC->SetRelativeLocation(FVector(0, 0, -90));
	MeshC->SetRelativeRotation(FRotator(0, -90, 0));
	if (SkeletalMesh.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalMesh.Object); }

	// 애니메이션 BP 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(
		TEXT("/Game/Blueprints/ABP_MainCharacter.ABP_MainCharacter_C"));
	if (AnimBP.Succeeded()) { MeshC->SetAnimInstanceClass(AnimBP.Class); }

	// 카메라 설정
	SpringArmC = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmC->SetupAttachment(RootComponent);
	SpringArmC->TargetArmLength = 330.0f;
	CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraC->SetRelativeLocation(FVector(0, 50, 0));
	CameraC->SetRelativeRotation(FRotator(12, 0, 0));
	CameraC->PostProcessSettings.MotionBlurAmount = 0.0f;
	CameraC->SetupAttachment(SpringArmC);

	// 카메라 회전 설정
	SpringArmC->bUsePawnControlRotation = true; //마우스 회전을 따라감
	CameraC->bUsePawnControlRotation = false; //스프링 암의 회전에 의존
	bUseControllerRotationYaw = false; //마우스 회전값을 플레이어 방향에 반영하지 않음
	GetCharacterMovement()->bOrientRotationToMovement = true; //메시를 이동방향으로 계속 갱신하는 기능 사용
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0); //갱신 속도를 초당 720도로 설정

	// 카메라 Lag 설정
	SpringArmC->bEnableCameraLag = true;
	SpringArmC->CameraLagSpeed = 50.0f;
	SpringArmC->bEnableCameraRotationLag = true;
	SpringArmC->CameraRotationLagSpeed = 50.0f;

	// // 건 메시 설정
	// GunMeshC = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
	// GunMeshC->SetupAttachment(MeshC);
	// GunMeshC->SetRelativeLocation(FVector(-14, 52, 120));
	// static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMesh(
	// 	TEXT("/Game/Features/FPWeapon/Mesh/SK_FPGun.SK_FPGun"));
	// if (GunMesh.Succeeded()) { GunMeshC->SetSkeletalMesh(GunMesh.Object); }
	//
	// // 스나이퍼 메시 설정
	// SniperMeshC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperStaticMeshComponent"));
	// SniperMeshC->SetupAttachment(MeshC);
	// SniperMeshC->SetRelativeLocation(FVector(-22, 55, 120));
	// SniperMeshC->SetRelativeScale3D(FVector(0.15f));
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> SniperMesh(TEXT("/Game/Features/SniperGun/sniper1.sniper1"));
	// if (SniperMesh.Succeeded()) { SniperMeshC->SetStaticMesh(SniperMesh.Object); }

	// 배트 메시 설정
	BatMeshC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BatStaticMeshComponent"));
	BatMeshC->SetupAttachment(MeshC, TEXT("hand_r_socket"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BatMesh(
		TEXT("/Game/Features/Weapons_Free/Meshes/SM_baseball_bat_001.SM_baseball_bat_001"));
	if (BatMesh.Succeeded()) { BatMeshC->SetStaticMesh(BatMesh.Object); }

	// 전투 컴포넌트 설정
	CombatC = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage(
		TEXT("/Game/Features/Mannequin/Animations/Montage/AttackMontage.AttackMontage"));
	if (AttackMontage.Succeeded()) { CombatC->AttackMontage = AttackMontage.Object; }
	CombatC->AttackMontageSections = {FName("1"), FName("2"), FName("3"), FName("4")};
	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontage(
		TEXT("/Game/Features/Mannequin/Animations/Montage/RollMontage.RollMontage"));
	if (RollMontage.Succeeded()) { CombatC->RollMontage = RollMontage.Object; }

	FocusingC = CreateDefaultSubobject<UFocusingComponent>(TEXT("FocusingComponent"));
	
	HealthC = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StaminaC = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));

	// 불릿 팩토리 설정
	static ConstructorHelpers::FClassFinder<ABaseBullet> BaseBulletBP(
		TEXT("/Game/Blueprints/BP_BaseBullet.BP_BaseBullet_C"));
	if (BaseBulletBP.Succeeded()) { BulletF = BaseBulletBP.Class; }

	// 불릿 이펙트 팩토리 설정
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletEffect(
		TEXT("/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect"));
	if (BulletEffect.Succeeded()) { BulletEffectF = BulletEffect.Object; }

	// 스나이퍼 UI 설정
	// static ConstructorHelpers::FClassFinder<UUserWidget> SniperUiBP(
	// 	TEXT("/Game/UI/BP_SniperUi.BP_SniperUi_C"));
	// if (SniperUiBP.Succeeded()) { SniperUI_W = SniperUiBP.Class; }

	// UI 클래스 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> MiniMap(TEXT("/Game/UI/WBP_Minimap.WBP_Minimap_C"));
	if (MiniMap.Succeeded()) { MiniMapW = MiniMap.Class; }
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUD(TEXT("/Game/UI/WBP_PlayerHUD.WBP_PlayerHUD_C"));
	if (PlayerHUD.Succeeded()) { PlayerHUD_W = PlayerHUD.Class; }
	static ConstructorHelpers::FClassFinder<UUserWidget> GameAlert(TEXT("/Game/UI/WBP_GameAlertUI.WBP_GameAlertUI_C"));
	if (GameAlert.Succeeded()) { GameAlertUI_W = GameAlert.Class; }

	// 팩토리 설정
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DamagedEffect(
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DamagedEffect.Succeeded()) { DamagedFxF = DamagedEffect.Object; }
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParriedEffect(
		TEXT("/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect"));
	if (ParriedEffect.Succeeded()) { ParriedFxF = ParriedEffect.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> DamagedSound(
		TEXT("/Game/Features/BaseBallBatSound/bat_wood.bat_wood"));
	if (DamagedSound.Succeeded()) { DamagedSfxF = DamagedSound.Object; }
	static ConstructorHelpers::FObjectFinder<USoundBase> ParriedSound(
		TEXT("/Game/Features/BaseBallBatSound/critical_hit.critical_hit"));
	if (ParriedSound.Succeeded()) { ParriedSfxF = ParriedSound.Object; }

	// 점프 수치 설정
	const auto CharMoveC = GetCharacterMovement();
	CharMoveC->JumpZVelocity = 500.0f;
	CharMoveC->AirControl = 1.0f;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeMiniMap(); // 미니맵 생성 함수 호출
	InitializePlayerHUD(); // 유저 상태 생성 함수 호출
	InitializeGameAlert(); // 유저 상태 생성 함수 호출

	// _sniperUI = CreateWidget(GetWorld(), SniperUiF);
	// InputChangeSniperGun();

	if (auto Widget = Cast<UPlayerHUDWidget>(PlayerHUDWidget))
	{
		HealthC->OnHealthChanged.AddDynamic(Widget, &UPlayerHUDWidget::HandleHealthChanged);
		StaminaC->OnStaminaChanged.AddDynamic(Widget, &UPlayerHUDWidget::HandleStaminaChanged);
	}
	HealthC->OnDeath.AddDynamic(this, &AMainCharacter::HandleDeath);

	// 전투 컴포넌트의 델리게이트들을 토대로 시각적 처리
	CombatC->OnDamaged.AddDynamic(this, &AMainCharacter::HandleDamaged);
	CombatC->OnParried.AddDynamic(this, &AMainCharacter::HandleParried);
	CombatC->OnStaggered.AddDynamic(this, &AMainCharacter::HandleStaggered);
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickMovement(DeltaTime);
	TickStamina(DeltaTime);
	TickFocusControl(DeltaTime);
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::InputJump);
	// PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainCharacter::InputFire);
	// PlayerInputComponent->BindAction("GrenadeGun", IE_Pressed, this, &AMainCharacter::InputChangeGrenadeGun);
	// PlayerInputComponent->BindAction("SniperGun", IE_Pressed, this, &AMainCharacter::InputChangeSniperGun);
	// PlayerInputComponent->BindAction("SniperAim", IE_Pressed, this, &AMainCharacter::InputSniperAim);
	// PlayerInputComponent->BindAction("SniperAim", IE_Released, this, &AMainCharacter::InputSniperAim);

	// 전투 컴포넌트 바인드
	PlayerInputComponent->BindAction("Attack", IE_Pressed, CombatC, &UCombatComponent::Attack);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &AMainCharacter::Roll);
	PlayerInputComponent->BindAction("Parry", IE_Pressed, CombatC, &UCombatComponent::Parry);

	PlayerInputComponent->BindAction("Focus", IE_Pressed, FocusingC, &UFocusingComponent::CycleTarget);
}

void AMainCharacter::Turn(const float Value)
{
	if (!bOverControl) AddControllerYawInput(Value);
}
void AMainCharacter::LookUp(const float Value)
{
	if (!bOverControl) AddControllerPitchInput(Value);
}
void AMainCharacter::MoveForward(const float Value)
{
	if (!bOverMove) InputDirection.X = Value;
}
void AMainCharacter::MoveRight(const float Value)
{
	if (!bOverMove) InputDirection.Y = Value;
}

void AMainCharacter::InputJump()
{
	const bool bIsJumping = GetCharacterMovement()->IsFalling();
	if (!bIsJumping && StaminaC->CurrentStamina > 10.0f)
	{
		Super::Jump();

		StaminaC->UpdateStamina(-10.0f);
	}
}

// void AMainCharacter::InputFire()
// {
// 	if (bUsingGrenadeGun)
// 	{
// 		const FTransform FireLocation = GunMeshC->GetSocketTransform(TEXT("FireLocation"));
// 		GetWorld()->SpawnActor<ABaseBullet>(BulletF, FireLocation);
// 	}
// 	else
// 	{
// 		FVector StartLocation = CameraC->GetComponentLocation();
// 		FVector EndLocation = StartLocation + CameraC->GetForwardVector() * 5000.0f;
// 		FHitResult HitInfo;
// 		FCollisionQueryParams Params;
// 		Params.AddIgnoredActor(this);
// 		bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartLocation, EndLocation, ECC_Visibility, Params);
// 		if (bHit)
// 		{
// 			FTransform BulletTransform;
// 			BulletTransform.SetLocation(HitInfo.ImpactPoint);
// 			PRINT_LOG(TEXT("Bullet Transform : %s"), *BulletTransform.GetLocation().ToString());
// 			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletEffectF, BulletTransform);
// 		}
// 	}
// }
//
// void AMainCharacter::InputChangeGrenadeGun()
// {
// 	bUsingGrenadeGun = true;
// 	SniperMeshC->SetVisibility(false);
// 	GunMeshC->SetVisibility(true);
// }
//
// void AMainCharacter::InputChangeSniperGun()
// {
// 	bUsingGrenadeGun = false;
// 	SniperMeshC->SetVisibility(true);
// 	GunMeshC->SetVisibility(false);
// }
//
// void AMainCharacter::InputSniperAim()
// {
// 	if (bUsingGrenadeGun) { return; }
// 	if (bSniperAim == false)
// 	{
// 		bSniperAim = true;
// 		_sniperUI->AddToViewport();
// 		CameraC->SetFieldOfView(45.0f);
// 	}
// 	else
// 	{
// 		bSniperAim = false;
// 		_sniperUI->RemoveFromViewport();
// 		CameraC->SetFieldOfView(90.0f);
// 	}
// }

void AMainCharacter::Roll()
{
	// 유휴, 이동 이외의 상태일 경우 회피 불가능
	if (CombatC->CombatState != ECombatState::IdleMoving) return;

	if (StaminaC->CurrentStamina < 10.0f) return;

	SetOverMove(true);
	const FVector InputVector = FVector(InputDirection.X, InputDirection.Y, 0);
	if (InputVector == FVector::ZeroVector)
	{
		// 제자리에서 구를 경우
		SetOverMoveParams(GetActorForwardVector(), 300.0f);
	}
	else
	{
		// 방향 입력 중에 구를 경우
		bUseControllerRotationYaw = true;
		FaceRotation(InputVector.Rotation());
		bUseControllerRotationYaw = false;
		SetOverMoveParams(InputVector, 300.0f);
	}
	
	CombatC->SetCombatState(ECombatState::Rolling);
	// 구르기 몽타주 설정
	PlayAnimMontage(CombatC->RollMontage);
	// 무적시간 종료 타이머
	GetWorld()->GetTimerManager().SetTimer(CombatC->StateTimerHandle, [this]()
	{
		CombatC->SetCombatState(ECombatState::BufferTime);
	}, 0.2f, false);

	StaminaC->UpdateStamina(-10.0f);
}

void AMainCharacter::TickMovement(float DeltaTime)
{
	if (bOverMove)
	{
		SetActorLocation(GetActorLocation() + OverMoveDirection * OverMoveScale * DeltaTime);
		AddMovementInput(OverMoveDirection, OverMoveScale * DeltaTime);
	}
	else if (bIgnoreMove) {}
	else
	{
		InputDirection = InputDirection.GetRotated(GetControlRotation().Yaw);
		const FVector MoveDirection = FVector(InputDirection.X, InputDirection.Y, 0);
		AddMovementInput(MoveDirection);
	}
}

void AMainCharacter::TickStamina(float DeltaTime)
{
	StaminaC->UpdateStamina(DeltaTime * StaminaRecoveryRate);
}

void AMainCharacter::TickFocusControl(float DeltaTime)
{
	if (bOverControl && TargetFocusedC && TargetFocusedC->GetOwner())
	{
		const FVector CameraL = GetActorLocation();
		const FVector TargetL = TargetFocusedC->GetOwner()->GetActorLocation();
		FRotator DesiredR = UKismetMathLibrary::FindLookAtRotation(CameraL, TargetL);
		DesiredR.Add(-25, 0, 0);

		FRotator OldR = GetControlRotation();
		FRotator NewR = FMath::RInterpTo(OldR, DesiredR, DeltaTime, 10.0f);

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetControlRotation(NewR);
		}
	}
}

// 미니맵 생성 함수
void AMainCharacter::InitializeMiniMap()
{
	// 미니맵 1. 미니맵 위젯 생성
	if (MiniMapW)
	{
		MiniMapWidget = CreateWidget<UUserWidget>(GetWorld(), MiniMapW);
		if (MiniMapWidget)
		{
			MiniMapWidget->AddToViewport();
		}
	}

	// 미니맵 2. 월드에서 MinimapCaptureActor를 찾는다
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMinimapCaptureActor::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		AMinimapCaptureActor* MinimapActor = Cast<AMinimapCaptureActor>(FoundActors[0]);
		if (MinimapActor)
		{
			MinimapActor->SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, 2000.f));
			MinimapActor->SetActorRotation(FRotator(-90.f, 0.f, 0.f));
		}
	}
}

void AMainCharacter::InitializePlayerHUD()
{
	if (PlayerHUD_W)
	{
		PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUD_W);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();

			// 처음 상태 반영
			if (UPlayerHUDWidget* PlayerHUD = Cast<UPlayerHUDWidget>(PlayerHUDWidget))
			{
				PlayerHUD->HandleHealthChanged(HealthC->CurrentHealth, HealthC->MaxHealth);
				PlayerHUD->HandleStaminaChanged(StaminaC->CurrentStamina, StaminaC->MaxStamina);
				PlayerHUD->SetGold(CurrentGold);
			}
		}
	}
}

void AMainCharacter::InitializeGameAlert()
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

void AMainCharacter::ManageGold(int32 Delta)
{
	// 나중에 골드 음수 처리해야 할듯, 구매 못하게
	CurrentGold = FMath::Max(0, CurrentGold + Delta);

	if (UPlayerHUDWidget* PlayerHUD = Cast<UPlayerHUDWidget>(PlayerHUDWidget))
	{
		PlayerHUD->SetGold(CurrentGold);
	}
}

void AMainCharacter::ShowDeathUI()
{
	if (UGameAlertUIWidget* AlertUI = Cast<UGameAlertUIWidget>(GameAlertUIWidget))
	{
		AlertUI->SetVisibility(ESlateVisibility::Visible);
		AlertUI->PlayAlert(TEXT("LOTUS WITHERED"), FLinearColor::Red);
	}
}

void AMainCharacter::HandleDeath()
{
	// TODO: 몽타주 재생 추가
	ShowDeathUI();
}

void AMainCharacter::HandleDamaged()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedFxF, GetActorLocation());
	UGameplayStatics::SpawnSound2D(GetWorld(), DamagedSfxF);
}

void AMainCharacter::HandleParried()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParriedFxF,
	                                         GetMesh()->GetSocketLocation(TEXT("hand_l_socket")));
	UGameplayStatics::SpawnSound2D(GetWorld(), ParriedSfxF);
}

void AMainCharacter::HandleStaggered()
{
}

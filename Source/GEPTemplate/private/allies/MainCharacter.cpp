#include "allies/MainCharacter.h"

#include "GEPTemplate.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "allies/BaseBullet.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "allies/MinimapCaptureActor.h"       // AMinimapCaptureActor 헤더 포함


AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 기본 메시 포인터 로드
	USkeletalMeshComponent* MeshC = GetMesh();

	// 충돌 설정
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Character"));
	
	// 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT("/Game/Features/Mannequin/Character/rp_manuel_rigged_001_ue4/rp_manuel_rigged_001_ue4.rp_manuel_rigged_001_ue4"));
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
	CameraC->SetRelativeRotation(FRotator(12,0,0));
	CameraC->SetupAttachment(SpringArmC);

	// 카메라 회전 설정
	SpringArmC->bUsePawnControlRotation = true;	//마우스 회전을 따라감
	CameraC->bUsePawnControlRotation = false;	//스프링 암의 회전에 의존
	bUseControllerRotationYaw = false;	//마우스 회전값을 플레이어 방향에 반영하지 않음
	GetCharacterMovement()->bOrientRotationToMovement = true;	//메시를 이동방향으로 계속 갱신하는 기능 사용
	GetCharacterMovement()->RotationRate = FRotator(0, 360, 0);	//갱신 속도를 초당 360도로 설정

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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BatMesh(TEXT("/Game/Features/Weapons_Free/Meshes/SM_baseball_bat_001.SM_baseball_bat_001"));
	if (BatMesh.Succeeded()) { BatMeshC->SetStaticMesh(BatMesh.Object); }
	
	// 전투 컴포넌트 설정
	CombatC = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage(
		TEXT("/Game/Features/Mannequin/Animations/AttackMontage.AttackMontage"));
	if (AttackMontage.Succeeded()) { CombatC->AttackMontage = AttackMontage.Object; }
	CombatC->AttackMontageSections = { FName("1"), FName("2"), FName("3"), FName("4") };
	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontage(
		TEXT("/Game/Features/Mannequin/Animations/RollMontage.RollMontage"));
	if (RollMontage.Succeeded()) { CombatC->RollMontage = RollMontage.Object; }

	// 불릿 팩토리 설정
	static ConstructorHelpers::FClassFinder<ABaseBullet> BaseBulletBP(
		TEXT("/Game/Blueprints/BP_BaseBullet.BP_BaseBullet_C"));
	if (BaseBulletBP.Succeeded()) { BulletF = BaseBulletBP.Class; }

	// 스나이퍼 UI 팩토리 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> SniperUiBP(
		TEXT("/Game/UI/BP_SniperUi.BP_SniperUi_C"));
	if (SniperUiBP.Succeeded()) { SniperUiF = SniperUiBP.Class; }

	// 불릿 이펙트 팩토리 설정
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletEffect(
		TEXT("/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect"));
	if (BulletEffect.Succeeded()) { BulletEffectF = BulletEffect.Object; }

	// 점프 수치 설정
	const auto CharMoveC = GetCharacterMovement();
	CharMoveC->JumpZVelocity = 500.0f;
	CharMoveC->AirControl = 1.0f;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

    InitializeMiniMap(); // 미니맵 생성 함수 호출
	
	// _sniperUI = CreateWidget(GetWorld(), SniperUiF);
	// InputChangeSniperGun();
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickMovement();
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
	PlayerInputComponent->BindAction("Roll", IE_Pressed, CombatC, &UCombatComponent::Roll);
	PlayerInputComponent->BindAction("Parry", IE_Pressed, CombatC, &UCombatComponent::Parry);
}

void AMainCharacter::Turn(const float Value) { AddControllerYawInput(Value); }
void AMainCharacter::LookUp(const float Value) { AddControllerPitchInput(Value); }
void AMainCharacter::MoveForward(const float Value) { InputDirection.X = Value; }
void AMainCharacter::MoveRight(const float Value) { InputDirection.Y = Value; }
void AMainCharacter::InputJump() { Jump(); }

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

void AMainCharacter::TickMovement()
{
	InputDirection = InputDirection.GetRotated(GetControlRotation().Yaw);
	const FVector MoveDirection = FVector(InputDirection.X, InputDirection.Y, 0);
	AddMovementInput(MoveDirection);
}

// 미니맵 생성 함수
void AMainCharacter::InitializeMiniMap()
{
    // 미니맵 1. 미니맵 위젯 생성
    if (MiniMapWidgetClass)
    {
        MiniMapWidget = CreateWidget<UUserWidget>(GetWorld(), MiniMapWidgetClass);
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
            MinimapActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            MinimapActor->SetActorRelativeLocation(FVector(0.f, 0.f, 2000.f));
            MinimapActor->SetActorRelativeRotation(FRotator(-90.f, 0.f, 0.f));
        }
    }
}

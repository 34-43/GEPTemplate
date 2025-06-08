#include "enemies/bosses/StatueBossHand.h"

#include "allies/MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "enemies/bosses/HandAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"


AStatueBossHand::AStatueBossHand()
{
	// 포인터 캐싱
	USkeletalMeshComponent* MeshC = GetMesh();
	UCapsuleComponent* CapsuleC = GetCapsuleComponent();
	UCharacterMovementComponent* CharMoveC = GetCharacterMovement();

	// 전체 크기 스케일
	SetActorScale3D(FVector(10.0f));

	// 메시 크기,위치,회전,충돌 설정
	MeshC->SetRelativeLocation(FVector(0,0,-50));
	MeshC->SetRelativeRotation(FRotator(0,-90,10));
	MeshC->SetRelativeScale3D(FVector(0.5f));
	MeshC->SetCollisionProfileName("NoCollision");
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalHand(TEXT("/Game/Features/Boss/CyberStatue/source/Hand.Hand"));
	if (SkeletalHand.Succeeded()) { MeshC->SetSkeletalMesh(SkeletalHand.Object); }

	// ABP 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> HandABP(TEXT("/Game/Features/Boss/CyberStatue/animation/ABP_Hand.ABP_Hand_C"));
	if (HandABP.Succeeded()) { MeshC->SetAnimInstanceClass(HandABP.Class); }

	// 캡슐 설정
	CapsuleC->SetCapsuleHalfHeight(35.0f);
	CapsuleC->SetCapsuleRadius(15.0f);
	CapsuleC->SetCollisionProfileName("Character");

	// 캐릭터 무브먼트 설정
	CharMoveC->GravityScale = 0.0f;
	CharMoveC->SetMovementMode(MOVE_Flying);

	// 조명 설정
	PointLightC = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	PointLightC->SetupAttachment(CapsuleC);
	PointLightC->SetIntensity(100000.0f);
	PointLightC->SetAttenuationRadius(500.0f);
	PointLightC->SetRelativeLocation(FVector(20,0,0));

	// 동적 생성 시 AIController 생성 여부
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AStatueBossHand::BeginPlay()
{
	Super::BeginPlay();
	ABPCache = Cast<UHandAnimInstance>(GetMesh()->GetAnimInstance());
}

void AStatueBossHand::PoundAndSpawn(const FOnActionCompleted& OnCompleted)
{
	OnPatternCompleted = OnCompleted;
	PoundStraight(FOnActionCompleted::CreateLambda([this]()
	{
		OnPatternCompleted.ExecuteIfBound();
	}));
}

void AStatueBossHand::BasicShot(const FOnActionCompleted& OnCompleted)
{
	OnPatternCompleted = OnCompleted;
	SetHandState(FRotator(0,0,180), EHandAnimState::Paper);
}

void AStatueBossHand::SetHandState(const FRotator& NewHandRotation, const EHandAnimState& NewState)
{
	SetActorRotation(NewHandRotation);
	ABPCache->SetAnimState(NewState);
}
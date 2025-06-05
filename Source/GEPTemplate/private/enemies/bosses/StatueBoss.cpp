#include "enemies/bosses/StatueBoss.h"

#include "GEPTemplate.h"
#include "allies/MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AStatueBoss::AStatueBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	// 캐싱
	UCharacterMovementComponent* CharMoveC = GetCharacterMovement();

	// 외형 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StatueMesh(
		TEXT("/Game/Features/Boss/CyberStatue/source/CyberStatue.CyberStatue"));
	if (StatueMesh.Succeeded()) { StaticMeshC->SetStaticMesh(StatueMesh.Object); }
	StaticMeshC->SetWorldScale3D(FVector(70.0f));
	StaticMeshC->SetRelativeLocation(FVector(0, -100, 0));
	
	StaticEyeC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh2"));
	StaticEyeC->SetupAttachment(StaticMeshC);
	StaticEyeC->SetWorldScale3D(FVector(0.045));
	StaticEyeC->SetRelativeLocation(FVector(2.714,4.571,25.142));
	StaticEyePupilC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh3"));
	StaticEyePupilC->SetupAttachment(StaticEyeC);
	StaticEyePupilC->SetRelativeScale3D(FVector(0.28));
	StaticEyePupilC->SetRelativeLocation(FVector(38,0,35));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereMesh.Succeeded())
	{
		StaticEyeC->SetStaticMesh(SphereMesh.Object);
		StaticEyePupilC->SetStaticMesh(SphereMesh.Object);
	}

	// 캐릭터 무브먼트 설정
	CharMoveC->GravityScale = 0.0f;
	CharMoveC->SetMovementMode(MOVE_Flying);

	// 팩토리 설정
	static ConstructorHelpers::FClassFinder<AStatueBossHand> StatueBossHandBP(
		TEXT("/Game/Blueprints/BP_StatueBossHand.BP_StatueBossHand_C"));
	if (StatueBossHandBP.Succeeded()) { BossHandF = StatueBossHandBP.Class; }

	// 조명 설정
	PointLightC = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	PointLightC->SetupAttachment(GetCapsuleComponent());
	PointLightC->SetIntensity(400000.0f);
	PointLightC->SetAttenuationRadius(2000.0f);
	PointLightC->SetRelativeLocation(FVector(1400, 0, 700));
}

void AStatueBoss::BeginPlay()
{
	Super::BeginPlay();
	InitializeHub();
}

void AStatueBoss::TriggerBossFight()
{
	Super::TriggerBossFight();
	InitializePatternCycle();
}

void AStatueBoss::InitializeHub()
{
	BossHands.Empty();

	for (int i = 0; i < BossHandsNum; i++)
	{
		AStatueBossHand* SpawnedHand = GetWorld()->SpawnActor<AStatueBossHand>(
			BossHandF, GetActorLocation(), GetActorRotation());
		BossHands.Add(SpawnedHand);
	}

	// 월드 위치 배열 초기화
	for (int i = 0; i < BossHandsNum; i++)
	{
		BossHandsWorldLocations.Add(GetActorRotation().RotateVector(BossHandsLocations[i]) + GetActorLocation());
	}
	
	for (int i = 0; i < BossHandsNum; i++)
	{
		if (BossHands[i] && BossHandsLocations.Num() >= i + 1)
		{
			// 월드 위치에 배치
			BossHands[i]->SetActorLocation(BossHandsWorldLocations[i]);

			// 상대 위치가 중앙 기준으로 음수에 있으면 메시 스케일링을 이용해 모양만 반대쪽 손으로 대칭시킴
			// if (BossHandsLocations[i].Y < 0)
			// 	BossHands[i]->GetMesh()->SetRelativeScale3D(
			// 		BossHands[i]->GetMesh()->GetRelativeScale3D() * FVector(-1, 1, 1));
		}
	}

	BossHandsControlFlags.Init(true, BossHands.Num());
}

void AStatueBoss::InitializePatternCycle()
{
	PatternBankIndex = 0;
	GetWorldTimerManager().SetTimer(StatueBossTimerHandle, [this]()
	{
		ExecuteNextCycle();
	}, 0.5f, false);
}

void AStatueBoss::ExecuteNextCycle()
{
	// 패턴 뱅크 초기화
	if (PatternBankIndex >= PatternBank.Num())
	{
		for (int32 i = PatternBank.Num() - 1; i > 0; --i)
		{
			const int32 j = FMath::RandRange(0, i);
			PatternBank.Swap(i, j);
		}
		PatternBankIndex = 0;
	}

	switch (PatternBank[PatternBankIndex])
	{
	case 0:
		ShockwavePattern();
		break;
	case 1:
		ExplosionPattern();
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		break;
	}
	PatternBankIndex++;
}

void AStatueBoss::ShockwavePattern()
{
	if (BossHands.Num() >= 2)
	{
		// 선도 손, 지연 손 index를 하나씩 고름.
		const int Leading = FMath::RandRange(0, BossHands.Num() - 1);
		const int PreLagging = FMath::RandRange(0, BossHands.Num() - 2);
		const int Lagging = PreLagging + ((Leading == PreLagging) ? 1 : 0);

		// 1. 선도 손 플레이어 위로 비행
		BossHandsControlFlags[Leading] = false;
		const FVector Loc1 = Player->GetActorLocation() + FVector(0, 0, 1000);
		BossHands[Leading]->FlyToLocation(Loc1, 4000.0f, FOnActionCompleted::CreateLambda([this, Leading, Lagging]()
		{
			// 2. 선도 손 첫번째 내리찍기
			BossHands[Leading]->SetExplosive(false);
			BossHands[Leading]->SetHandState(FRotator(-90,180,0), EHandAnimState::Paper);
			BossHands[Leading]->PoundAndSpawn(FOnActionCompleted::CreateLambda([this, Leading]()
			{
				// 3.5. 조금 띄우기
				BossHands[Leading]->FlyToLocation(BossHands[Leading]->GetActorLocation() + FVector(0, 0, 1000), 1500.0f, FOnActionCompleted::CreateLambda([this, Leading]()
				{
					// 4.5. 그자리에서 한번 더 내리찍기
					BossHands[Leading]->SetHandState(FRotator(-90,180,0), EHandAnimState::Paper);
					BossHands[Leading]->PoundAndSpawn(FOnActionCompleted::CreateLambda([this, Leading]()
					{
						// 5. 딜 타임
						GetWorldTimerManager().SetTimer(StatueBossTimerHandle, [this, Leading]()
						{
							// 5.5: 선도 손 기준으로, 복귀 및 다음 패턴으로 이동
							BossHands[Leading]->FlyToLocation(BossHandsWorldLocations[Leading], 4000.0f, FOnActionCompleted::CreateLambda([this, Leading]()
							{
								BossHandsControlFlags[Leading] = true;
								BossHands[Leading]->SetHandState(FRotator(0, 180, 0), EHandAnimState::Idle);
								ExecuteNextCycle();
							}));
						}, 2.5f, false);

					}));
				}));
			}));

			// 2. 지연 손 플레이어 위로 비행
			BossHandsControlFlags[Lagging] = false;
			const FVector Loc2 = Player->GetActorLocation() + FVector(0, 0, 1200);
			BossHands[Lagging]->FlyToLocation(Loc2, 3000.0f, FOnActionCompleted::CreateLambda([this, Lagging]()
			{
				// 3. 지연 손 내려찍기
				BossHands[Lagging]->SetExplosive(false);
				BossHands[Lagging]->SetHandState(FRotator(-90,180,0), EHandAnimState::Paper);
				BossHands[Lagging]->PoundAndSpawn(FOnActionCompleted::CreateLambda([this, Lagging]()
				{
					// 4. 지연 손은 알아서 먼저 복귀
					BossHands[Lagging]->SetHandState(FRotator(0, 180, 0), EHandAnimState::Idle);
					BossHands[Lagging]->FlyToLocation(BossHandsWorldLocations[Lagging], 2500.0f, FOnActionCompleted::CreateLambda([this, Lagging]()
					{
						BossHandsControlFlags[Lagging] = true;
					}));
				}));
			}));
		}));
	}
	else
	{
		ExecuteNextCycle();
	}
}

void AStatueBoss::ExplosionPattern()
{
	if (BossHands.Num() > 0)
	{
		// 1. 첫 번째 손 사출
		BossHandsControlFlags[0] = false;
		BossHands[0]->SetHandState(FRotator(180, 180, 0), EHandAnimState::Rock);
		const FVector Loc1 = Player->GetActorLocation() + FVector(0, 0, 1300);
		BossHands[0]->FlyToLocation(Loc1, 3000.0f, FOnActionCompleted::CreateLambda([this]()
		{
			BossHands[0]->SetExplosive(true);
			BossHands[0]->SetHandState(FRotator(180, 180, 0), EHandAnimState::Rock);
			BossHands[0]->PoundAndSpawn(FOnActionCompleted::CreateLambda([this]()
			{
				
			}));
			
			if (BossHands.Num() > 1)
			{
				// 2. 두 번째 손 사출
				BossHandsControlFlags[1] = false;
				const FVector Loc1 = Player->GetActorLocation() + FVector(0, 0, 1300);
				BossHands[1]->FlyToLocation(Loc1, 4000.0f, FOnActionCompleted::CreateLambda([this]()
				{
					ExecuteNextCycle();
				}));
			}
			else
			{
				ExecuteNextCycle();
			}
		}));
	}
	else
	{
		ExecuteNextCycle();
	}
}

void AStatueBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickHubMovement(DeltaTime);
}

void AStatueBoss::TickHubMovement(float DeltaTime)
{
	for (int i = 0; i < BossHandsNum; i++)
	{
		// 손이 없거나 제어중이 아니면 스킵
		if (!BossHands[i] || !BossHandsControlFlags[i]) continue;

		// 각도 설정
		FVector FacingDirection = Player->GetActorLocation() - BossHands[i]->GetActorLocation();
		BossHands[i]->SetActorRotation(FacingDirection.Rotation());
	}
}

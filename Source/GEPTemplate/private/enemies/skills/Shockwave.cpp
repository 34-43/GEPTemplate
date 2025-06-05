#include "enemies/skills/Shockwave.h"

#include "GEPTemplate.h"
#include "allies/MainCharacter.h"
#include "components/CombatComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"


AShockwave::AShockwave()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 세팅
	DecalC = CreateDefaultSubobject<UDecalComponent>(TEXT("GroundDecal"));
	SetRootComponent(DecalC);
	DecalC->SetWorldRotation(FRotator(-90, 0, 0));
	DecalC->DecalSize = FVector(CollisionHeightHalf, 10000, 10000);

	static ConstructorHelpers::FObjectFinder<UMaterial> DecalDMO(TEXT("/Game/Material/M_Shockwave_Dynamic.M_Shockwave_Dynamic"));
	if (DecalDMO.Succeeded()) { DecalC->SetDecalMaterial(DecalDMO.Object); }
}

void AShockwave::BeginPlay()
{
	Super::BeginPlay();

	ElapsedTime = 0.0f;
	Player = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass()));

	// 동적 데칼 머티리얼 지정
	if (DecalC && DecalC->GetDecalMaterial())
	{
		DecalDynamicMaterial = UMaterialInstanceDynamic::Create(DecalC->GetDecalMaterial(), this);
		DecalC->SetDecalMaterial(DecalDynamicMaterial);

		// 2) 렌더러에 강제 갱신 신호
		DecalC->MarkRenderStateDirty();
		DecalC->ReregisterComponent();

		// 3) 보이는 상태 보장
		DecalC->SetVisibility(true);
	}
}

void AShockwave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	const float CurrOuterRadius = InitialOuterRadius + ExpansionSpeed * ElapsedTime;
	const float CurrInnerRadius = CurrOuterRadius - RadiusGap;

	if (DecalDynamicMaterial)
	{
		DecalDynamicMaterial->SetScalarParameterValue(TEXT("InnerRadius"), CurrInnerRadius);
		DecalDynamicMaterial->SetScalarParameterValue(TEXT("OuterRadius"), CurrOuterRadius);
	}

	if (Player && !bSingleHitFlag)
	{
		// 점프 뛰었는지 검사
		const float ZDiff = FMath::Abs(Player->GetActorLocation().Z - Player->GetDefaultHalfHeight() - GetActorLocation().Z);
		PRINT_LOG(TEXT("diff: %f, height: %f"), ZDiff, CollisionHeightHalf);
		if (ZDiff > CollisionHeightHalf) return;

		// 중심부터 플레이어까지 평면상의 직선거리 계산
		const FVector Delta = Player->GetActorLocation() - GetActorLocation();
		const float PlanarDist = FVector2D(Delta.X, Delta.Y).Size();

		// 직선거리가 현재 이동중인 내/외부 반지름 반경 사이에 위치했는지 확인하고, 데미지 적용
		PRINT_LOG(TEXT("Dist: %f, Inner: %f, Outer: %f"), PlanarDist, CurrInnerRadius, CurrOuterRadius);
		if (PlanarDist >= CurrInnerRadius && PlanarDist < CurrOuterRadius)
		{
			if (UCombatComponent* PlayerCombatC = Cast<UCombatComponent>(Player->GetComponentByClass(UCombatComponent::StaticClass())))
			{
				const FVector DamageDirection = Delta.GetSafeNormal();
				PlayerCombatC->Damage(DamageAmount, DamageDirection, nullptr);
			}
			bSingleHitFlag = true;
		}
	}

	// 최대 반경 도달 시 액터 제거
	if (CurrOuterRadius >= MaxOuterRadius)
	{
		Destroy();
	}
}


#include "enemies/skills/Explosion.h"

#include "GEPTemplate.h"
#include "allies/MainCharacter.h"
#include "components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


AExplosion::AExplosion()
{
	// 팩토리 세팅
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEffect(
		TEXT("/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect"));
	if (ExplosionEffect.Succeeded()) { ExplosionFxF = ExplosionEffect.Object; }

	InitialLifeSpan = 1.0f;
}

void AExplosion::BeginPlay()
{
	Super::BeginPlay();

	const float Scale = ExplosionRadius / 20.0f;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFxF, GetActorLocation(),
	                                         FRotator::ZeroRotator, FVector(Scale));

	// todo: 액터 사용량 증가 시 최적화 필요
	Player = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (Player)
	{
		const FVector Delta = Player->GetActorLocation() - GetActorLocation();
		const float LineDist = Delta.Size();

		PRINT_LOG(TEXT("LineDist: %f"), LineDist);
		
		if (LineDist <= ExplosionRadius)
		{
			if (UCombatComponent* PlayerCombatC = Cast<UCombatComponent>(
				Player->GetComponentByClass(UCombatComponent::StaticClass())))
			{
				PlayerCombatC->Damage(DamageAmount, Delta.GetSafeNormal(), nullptr);
			}
		}
	}
}

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosion.generated.h"

UCLASS()
class GEPTEMPLATE_API AExplosion : public AActor
{
	GENERATED_BODY()

public:
	AExplosion();

protected:
	virtual void BeginPlay() override;

public:
	// 팩토리
	UPROPERTY(VisibleAnywhere) UParticleSystem* ExplosionFxF;
	// todo: 폭발 사운드 추가

	// 파라미터
	UPROPERTY(VisibleAnywhere, Category="Explosion") float ExplosionRadius = 900.0f;
	UPROPERTY(VisibleAnywhere, Category="Explosion") int32 DamageAmount = 50;

private:
	UPROPERTY() class AMainCharacter* Player;
};

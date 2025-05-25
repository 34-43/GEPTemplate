// ExplosiveBarrel.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class UInteractionComponent;
class UHealthComponent;
class URadialForceComponent;

UCLASS()
class GEPTEMPLATE_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 드럼통 메쉬 (Static Mesh)
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;
	// 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere) UInteractionComponent* InteractC;

	// 이펙트, 사운드 관련
	UPROPERTY(EditDefaultsOnly, Category = Effects)	UParticleSystem* ExplosionEffect;
	UPROPERTY(EditDefaultsOnly, Category = Effects)	USoundBase* ExplosionSound;

	
	// 폭발력 컴포넌트
	UPROPERTY(VisibleAnywhere) URadialForceComponent* RadialForce;
	// 파편 메시 6개 (자식 컴포넌트)
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Fragment0;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Fragment1;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Fragment2;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Fragment3;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Fragment4;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Fragment5;

	// 폭발 반경
	UPROPERTY(EditAnywhere, Category = "Explosion") float ExplosionRadius = 400.f;
	// 폭발 데미지
	UPROPERTY(EditAnywhere, Category = "Explosion")	float ExplosionDamage = 25.f;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
public:
	// 상호작용 함수 (InteractionComponent에서 호출)
	void Interact(AActor* Caller);
	UFUNCTION() void Explode();
	
private:
	FTimerHandle DestroyTimerHandle;
	void OnDestroyTimerExpired();
};

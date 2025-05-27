// ExplosiveBarrel.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

UCLASS()
class GEPTEMPLATE_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosiveBarrel();

	// 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere) class UInteractionComponent* InteractC;
	UPROPERTY(VisibleAnywhere) class UHealthComponent* HealthC;
	UPROPERTY(VisibleAnywhere) class UHealthFloatingComponent* HealthFloatingC;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 박스 컴포넌트
	UPROPERTY(EditDefaultsOnly) class UBoxComponent* BoxComp;
	// 드럼통 메쉬 (Static Mesh)
	UPROPERTY(EditDefaultsOnly) UStaticMeshComponent* Mesh;

	// 이펙트, 사운드 관련
	UPROPERTY(EditDefaultsOnly, Category = Effects)	UParticleSystem* ExplosionEffect;
	UPROPERTY(EditDefaultsOnly, Category = Effects)	USoundBase* ExplosionSound;

	
	// 폭발력 컴포넌트
	UPROPERTY(VisibleAnywhere) class URadialForceComponent* RadialForce;
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
	
public:
	// 상호작용 함수 (InteractionComponent에서 호출)
	void Interact(AActor* Caller);
	UFUNCTION() void Explode();

private:
	UFUNCTION()	void OnHealthChanged(int32 NewHealth, int32 MaxHealth);
	bool bHasExploded = false;
	UPROPERTY() APlayerController* MainPlayerController;
	FTimerHandle DestroyTimerHandle;
	void OnDestroyTimerExpired();
};

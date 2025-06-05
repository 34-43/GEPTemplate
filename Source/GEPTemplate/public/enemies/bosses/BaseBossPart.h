#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "enemies/skills/Explosion.h"
#include "enemies/skills/Shockwave.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseBossPart.generated.h"

DECLARE_DELEGATE(FOnActionCompleted);

UCLASS()
class GEPTEMPLATE_API ABaseBossPart : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseBossPart();

protected:
	UFUNCTION()
	virtual void BeginPlay() override;
	UFUNCTION() void OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void Tick(float DeltaTime) override;
	void TickMovement(float DeltaTime);

public:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere) class UHealthComponent* PartialHealthC;
	UPROPERTY(VisibleAnywhere) class UFocusedComponent* FocusedC;
	UPROPERTY(VisibleAnywhere) class UCombatComponent* CombatC;

	// 팩토리
	UPROPERTY(VisibleAnywhere) TSubclassOf<AShockwave> ShockwaveF;
	UPROPERTY(VisibleAnywhere) TSubclassOf<AExplosion> ExplosionF;
	// UPROPERTY(VisibleAnywhere) TSubclassOf<AAttackPath> AttackPathF; // 미구현

	// 파라미터
	UPROPERTY(VisibleAnywhere) float FlyAcceptance = 10.0f;
	
	// 중력이 없는 상태로 부드러운 직선 이동을 시킵니다.
	virtual void FlyToLocation(const FVector& Location, const float AverageSpeed, const FOnActionCompleted& OnCompleted);

	// 현재 위치에서 수직으로 낙하하여, 충돌하면 다음 동작을 수행합니다.
	void PoundStraight(const FOnActionCompleted& OnCompleted);

	// 충격 시 폭발할 지 (아니면 쇼크웨이브를 만들 지)
	void SetExplosive(const bool Value) { bIsExplosive = Value; }
	
	// 충격파를 만들어냅니다.
	void SpawnShockwave(int32 Damage, float ExpansionSpeed, FVector WorldLocation);

	// 폭발을 만들어냅니다.
	void SpawnExplosion(int32 Damage, float ExplosionRadius, FVector WorldLocation);

	// 일시적으로, 충돌에 의한 공격 판정만을 가지도록 설정합니다.
	void ActivateHitbox(int32 Damage, float LifeTime);

	// 충돌에 의한 피격 판정만을 가지도록 설정합니다. ActivateHitbox를 강제로 중지할 때 사용합니다.
	void DeactivateHitbox();

	// 공격 동선을 바닥에 그립니다.
	void AlertAttackPath(FVector From, FVector To, float LifeTime);
	
protected:
	// 플레이어 캐시. 서브 클래스에서도 필요할 때 참조할 수 있습니다.
	UPROPERTY() const AMainCharacter* Player;
	
private:
	// 로직
	FOnActionCompleted OnFlyCompleted;
	FOnActionCompleted OnPoundCompleted;
	FTimerHandle BaseBossPartTimerHandle;
	UFUNCTION() void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	// FlyToLocation 관련 로직
	bool bIsFlying = false;
	bool bBeforePound = false;
	bool bIsExplosive = false;
	FVector FlyTargetLocation;
	float FlyAverageSpeed = 0.0f;
	float FlyInitialDistance = 0.0f;
};

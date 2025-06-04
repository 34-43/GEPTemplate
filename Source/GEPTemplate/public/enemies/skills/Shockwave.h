#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shockwave.generated.h"

UCLASS()
class GEPTEMPLATE_API AShockwave : public AActor
{
	GENERATED_BODY()

public:
	AShockwave();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category="Shockwave") UDecalComponent* DecalC;
	
	// todo: 충격파 사운드 추가

	// 파라미터
	UPROPERTY(EditAnywhere, Category="Shockwave") float InitialOuterRadius = 0.0f;
	UPROPERTY(EditAnywhere, Category="Shockwave") float RadiusGap = 30.0f;
	UPROPERTY(EditAnywhere, Category="Shockwave") float MaxOuterRadius = 2000.0f;
	UPROPERTY(EditAnywhere, Category="Shockwave") float ExpansionSpeed = 600.0f;
	UPROPERTY(EditAnywhere, Category="Shockwave") float CollisionHeightHalf = 50.0f;
	UPROPERTY(EditAnywhere, Category="Shockwave") int32 DamageAmount = 25;

	// 동적 머티리얼
	UPROPERTY(EditAnywhere, Category="Shockwave") UMaterialInstanceDynamic* DecalDynamicMaterial;

private:
	UPROPERTY() class AMainCharacter* Player;
	float ElapsedTime = 0.0f;
	bool bSingleHitFlag = false;
};

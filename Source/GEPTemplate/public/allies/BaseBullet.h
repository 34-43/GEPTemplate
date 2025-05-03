#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBullet.generated.h"

UCLASS()
class GEPTEMPLATE_API ABaseBullet : public AActor
{
	GENERATED_BODY()

public:
	ABaseBullet();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 컴포넌트
	UPROPERTY(VisibleAnywhere) class USphereComponent* CollC;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* MeshC;
	UPROPERTY(VisibleAnywhere, Category = Movement) class UProjectileMovementComponent* ProjMoveC;

private:
	// 로직
	void BeginPlaySetDestroyTimer();
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ItemSample.generated.h"

UCLASS()
class GEPTEMPLATE_API AItemSample : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere) USphereComponent* CollisionC;
	UPROPERTY(VisibleAnywhere, Category = "Mesh") UStaticMeshComponent* MeshC;
	UPROPERTY(EditAnywhere, Category = "Visual") UMaterialInterface* CustomMaterial = nullptr;

	
public:	
	AItemSample();

	// 사용 종류
	UPROPERTY(EditAnywhere, Category="Item Effect") float HealthDelta = 0.f;
	UPROPERTY(EditAnywhere, Category="Item Effect")	float StaminaDelta = 0.f;
	UPROPERTY(EditAnywhere, Category="Item Effect")	int32 GoldDelta = 0;
	// 충돌
	UFUNCTION()	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// 아이템 사용
	UFUNCTION(BlueprintCallable) void Use(class AMainCharacter* TargetCharacter);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void OnConstruction(const FTransform& Transform);
};

// SavePoint.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SavePoint.generated.h"

class UStaticMeshComponent;
class UInteractionComponent;

UCLASS()
class GEPTEMPLATE_API ASavePoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASavePoint();

protected:
	virtual void BeginPlay() override;

	// 정수기 메쉬 (Static Mesh)
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;
	// 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere) UInteractionComponent* InteractC;

public:
	// 상호작용 함수 (InteractionComponent에서 호출)
	void Interact(AActor* Caller);

private:
	FTimerHandle ReenableTimerHandle;
	UFUNCTION()	void ReenablePower();
};

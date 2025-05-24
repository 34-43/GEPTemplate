// WaterDispenser.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterDispenser.generated.h"

class UStaticMeshComponent;
class UInteractionComponent;

UCLASS()
class GEPTEMPLATE_API AWaterDispenser : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWaterDispenser();

protected:
	virtual void BeginPlay() override;

	// 정수기 메쉬 (Static Mesh)
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;
	// 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere) UInteractionComponent* InteractC;

public:
	// 상호작용 함수 (InteractionComponent에서 호출)
	void Interact(AActor* Caller);
};
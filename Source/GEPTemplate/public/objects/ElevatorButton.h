// WaterDispenser.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElevatorButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS()
class GEPTEMPLATE_API AElevatorButton : public AActor
{
	GENERATED_BODY()

public:
	AElevatorButton();

protected:
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere) class UInteractionComponent* InteractC;

public:
	// 상호작용 함수 (InteractionComponent에서 호출)
	void Interact(AActor* Caller);

	// 상호작용 블루프린트 트리거
	UPROPERTY(BlueprintAssignable) FOnInteract OnInteract;

private:
	FTimerHandle ReenableTimerHandle;
	UFUNCTION()	void ReenablePower();
};
// InteractionComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UUserWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// UI와 테두리 On/Off
	UFUNCTION(BlueprintCallable) void ShowHighlight(bool bShow);
	UFUNCTION(BlueprintCallable) void ShowUI(bool bShow);
	
	bool IsInRange() const;
	void TryInteract();
	float GetDistanceToPlayer() const;

	// 공용 상호작용 거리 (모든 객체가 공유)
	static float InteractRange;
protected:
	virtual void BeginPlay() override;

	UPROPERTY()	AActor* PlayerRef = nullptr;
	UPROPERTY()	AActor* OwnerActor = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<UUserWidget> InteractionWidgetClass;
	
	UPROPERTY()	UUserWidget* InteractionWidgetInstance;
	UPROPERTY()	AActor* LastDetectedActor;
};
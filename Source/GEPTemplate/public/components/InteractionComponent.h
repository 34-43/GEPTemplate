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
	UInteractionComponent();
	AActor* GetOwnerActor() const;
	// UI와 테두리 On/Off
	UFUNCTION(BlueprintCallable) void ShowHighlight(bool bShow);
	UFUNCTION(BlueprintCallable) void ShowUI(bool bShow);
	bool IsInRange() const;
	void TryInteract();
	float GetDistanceToPlayer() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Interaction") float InteractRange = 250.0f;

	UPROPERTY()	AActor* PlayerRef = nullptr;
	UPROPERTY()	AActor* OwnerActor = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<UUserWidget> InteractionWidgetClass;
	
	UPROPERTY()	UUserWidget* InteractionWidgetInstance;
	UPROPERTY()	AActor* LastDetectedActor;
};
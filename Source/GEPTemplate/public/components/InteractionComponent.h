// InteractionComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UUserWidget;
class UImage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();
	// UI와 테두리 On/Off
	UFUNCTION(BlueprintCallable) void ShowHighlight(bool bShow);
	UFUNCTION(BlueprintCallable) void ShowUI(bool bShow);
	UFUNCTION(BlueprintCallable) void SetPower(bool bOnOff) { IsPowerOn  = bOnOff; }
	UFUNCTION(BlueprintCallable) void SetProgress(float Value);
	
	bool IsInRange() const;
	void TriggerInteraction();
	float GetDistanceToPlayer() const;
	
	// 각 상호작용 대상별 거리, 시간 (디폴트: 300cm, 0.8초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction") bool IsPowerOn  = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction") float InteractRange = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction") float InteractionDuration = 0.8f;
	float GetRequiredHoldTime() const { return InteractionDuration; }
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()	AActor* PlayerRef = nullptr;
	UPROPERTY()	AActor* OwnerActor = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<UUserWidget> InteractionWidgetClass;
	
	UPROPERTY()	UUserWidget* InteractionWidgetInstance;
	UPROPERTY()	AActor* LastDetectedActor;
	
	UPROPERTY()	UMaterialInstanceDynamic* ProgressMaterial;
	UPROPERTY()	UImage* ProgressImage;
};
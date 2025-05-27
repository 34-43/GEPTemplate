// HealthFloatingComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthFloatingComponent.generated.h"

class UWidgetComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UHealthFloatingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthFloatingComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	// 보이기, 숨기기
	UFUNCTION(BlueprintCallable) void ShowHealthBar();
	UFUNCTION(BlueprintCallable) void HideHealthBar();
	
private:
	// 플레이어 위치에 따라 업데이트
	void UpdateWidget();

	UPROPERTY(VisibleAnywhere)
	class UHealthComponent* HealthC;

	UPROPERTY()
	APlayerController* CachedPC;

	int FrameCounter = 0;
	bool bIsWidgetBound = false;

	UPROPERTY() UWidgetComponent* WidgetComponent;
	// 위젯 클래스는 에디터에서 지정하거나 생성자에서 지정 가능
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
};

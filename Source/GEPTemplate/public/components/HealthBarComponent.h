// HealthBarComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthBarComponent.generated.h"

class UWidgetComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UHealthBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthBarComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Called every frame
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

	// 위젯 위치 오프셋 (예: 캐릭터 머리 위)
	UPROPERTY(EditAnywhere, Category = "Widget")
	FVector WidgetRelativeLocation = FVector(0.f, 0.f, 120.f);
};
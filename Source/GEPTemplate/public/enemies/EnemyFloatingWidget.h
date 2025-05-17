#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyFloatingWidget.generated.h"

UCLASS()
class GEPTEMPLATE_API UEnemyFloatingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UProgressBar* HealthBar;

	UFUNCTION()
	void HandleHealthChanged(int32 NewHealth, int32 MaxHealth);
};

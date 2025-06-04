#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEPTEMPLATE_API UBossHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- 위젯 바인딩 ---
	UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_Health;

	// --- 데이터 갱신 함수 ---
	UFUNCTION() void HandleHealthChanged(int32 NewHealth, int32 MaxHealth);
};

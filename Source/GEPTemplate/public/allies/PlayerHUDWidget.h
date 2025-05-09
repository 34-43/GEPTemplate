#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

UCLASS()
class GEPTEMPLATE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- 위젯 바인딩 ---
	UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_Health;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_Stamina;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_GoldValue;

	// --- 데이터 갱신 함수 ---
	UFUNCTION(BlueprintCallable) void SetHealth(float Percent);
	UFUNCTION(BlueprintCallable) void SetStamina(float Percent);
	UFUNCTION(BlueprintCallable) void SetGold(int32 GoldAmount);
};

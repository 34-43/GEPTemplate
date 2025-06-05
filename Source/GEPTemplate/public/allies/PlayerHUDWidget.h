#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

UCLASS()
class GEPTEMPLATE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	// --- 위젯 바인딩 ---
	UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_Health;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* PB_Stamina;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_GoldValue;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_Item_1;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_Item_2;
	UPROPERTY(meta = (BindWidget)) class UImage* Img_Item_1;
	UPROPERTY(meta = (BindWidget)) class UImage* Img_Item_2;

	// --- 데이터 갱신 함수 ---
	UFUNCTION() void HandleHealthChanged(int32 NewHealth, int32 MaxHealth);
	UFUNCTION() void HandleStaminaChanged(float NewStamina, float MaxStamina);
	UFUNCTION(BlueprintCallable) void SetHealth(float Percent);
	UFUNCTION(BlueprintCallable) void SetStamina(float Percent);
	UFUNCTION(BlueprintCallable) void SetGold(int32 GoldAmount);

	// --- 아이템 관련 함수 ---
	UFUNCTION(BlueprintCallable) void SetItem1(int32 ItemAmount);
	UFUNCTION(BlueprintCallable) void SetItem2(int32 ItemAmount);
	UFUNCTION(BlueprintCallable) void SetItem1Opacity(bool bActive);
	UFUNCTION(BlueprintCallable) void SetItem2Opacity(bool bActive);
	UFUNCTION(BlueprintCallable) void SetItem1Progress(float Value);
	UFUNCTION(BlueprintCallable) void SetItem2Progress(float Value);
	UFUNCTION(BlueprintCallable) void UpdateItemHUD(int32 Index, int32 ItemAmount, float ProgressValue, bool bInCooldown);

protected:
	UPROPERTY()	UMaterialInstanceDynamic* ProgressMaterial1;
	UPROPERTY()	UMaterialInstanceDynamic* ProgressMaterial2;
	UPROPERTY(meta = (BindWidget)) UImage* Image_Progress_1;
	UPROPERTY(meta = (BindWidget)) UImage* Image_Progress_2;
};

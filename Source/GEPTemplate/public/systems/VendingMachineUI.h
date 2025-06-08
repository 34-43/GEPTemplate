// VendingMachineUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VendingMachineUI.generated.h"

class UTextBlock;
class UButton;
class AVendingMachine;
class AMainCharacter;

UCLASS()
class GEPTEMPLATE_API UVendingMachineUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	void LoadShop();

	UPROPERTY() AVendingMachine* OwningVendingMachine;
	UFUNCTION(BlueprintCallable) void CloseShop();
	UFUNCTION(BlueprintCallable) void BuyItem0() { BuyItem(0); }
	UFUNCTION(BlueprintCallable) void BuyItem1() { BuyItem(1); }

protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* CoinText;
	UPROPERTY(meta = (BindWidget)) UButton* CloseButton;
	// 상점 버튼
	UPROPERTY(meta = (BindWidget)) UButton* BuyButton0;
	UPROPERTY(meta = (BindWidget)) UButton* BuyButton1;
	// 가격표
	UPROPERTY(meta = (BindWidget)) UTextBlock* PriceText0;
	UPROPERTY(meta = (BindWidget)) UTextBlock* PriceText1;
	void BuyItem(int32 ItemCode);
	// 플레이어 저장
	UPROPERTY()	AMainCharacter* PlayerRef = nullptr;
	int32 Coin = 0;
	int32 Price[2] = { 5, 2 };
};

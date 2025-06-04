// ConfirmPopup.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmPopup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmPopupConfirmed);

class UButton;
class UTextBlock;

UCLASS()
class GEPTEMPLATE_API UConfirmPopup : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Popup") FOnConfirmPopupConfirmed OnConfirmed;
	// 팝업의 안내 텍스트를 설정
	void SetInfoText(const FText& NewText);
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION() void OnConfirmClicked();
	UFUNCTION() void OnCloseClicked();
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* InfoText;
	
	UPROPERTY(meta = (BindWidget)) UButton* YesButton;
	UPROPERTY(meta = (BindWidget)) UButton* NoButton;
	UPROPERTY(meta = (BindWidget)) UButton* CloseButton;
};

// StartMenuBT.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuBT.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class GEPTEMPLATE_API UStartMenuBT : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 버튼의 텍스트를 바꾸는 함수
	void SetButtonText(const FText& NewText);

	// 버튼 자체를 가져오는 함수 (바인딩용)
	UButton* GetButton() const { return Menu_Button; }

protected:
	// 블루프린트에 있는 Text와 Button을 연결
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Button;
	UPROPERTY(meta = (BindWidget)) UButton* Menu_Button;
};

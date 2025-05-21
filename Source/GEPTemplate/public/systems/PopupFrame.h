// PopupFrame.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupFrame.generated.h"
/**
 * 설정창은 메뉴화면에서 로딩된 후 삭제하지 않습니다
 * 숨김 처리 해서 나중에 Esc 키 누른 후 설정 변경과 도움말 버튼 눌러서 다시 볼 수 있습니다
 */
class UButton;
class UTextBlock;
class UVerticalBox;
class UWidget;

UCLASS()
class GEPTEMPLATE_API UPopupFrame : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPages(const TArray<UUserWidget*>& InPages, const TArray<FText>& InTitles);
	
protected:
	virtual void NativeConstruct() override;
	virtual void InitializePages();
	// 페이지 변경 로직
	void ShowPage(int32 PageIndex);
	// 현재 페이지에 따른 버튼 비활성화
	void UpdateNavigationVisibility();

	// 버튼 콜백
	UFUNCTION() void OnPrevClicked();
	UFUNCTION() void OnNextClicked();
	UFUNCTION() void OnCloseClicked();

	// 페이지 컨테이너
	UPROPERTY(meta = (BindWidget)) UVerticalBox* PageContainer;
	UPROPERTY() TArray<UUserWidget*> Pages;
	UPROPERTY() TArray<FText> PageTitles;
protected:
	UPROPERTY(meta = (BindWidget)) UButton* PrevButton;
	UPROPERTY(meta = (BindWidget)) UButton* NextButton;
	UPROPERTY(meta = (BindWidget)) UButton* CloseButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TitleText;

	// 현재 페이지
	int32 CurrentPageIndex = 0;
};
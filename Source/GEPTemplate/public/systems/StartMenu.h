// StartMenu.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenu.generated.h"

class UVerticalBox;
class UPopupFrame;
class UConfirmPopup;

UCLASS()
class GEPTEMPLATE_API UStartMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// 위젯의 시작 시 호출되는 함수
	virtual void NativeConstruct() override;

	// 메뉴 버튼을 동적으로 추가하는 함수
	void CreateMenuButtons();

protected:
	// VerticalBox 위젯
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* BT_List;
	// WBP_MenuButton 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> WBP_MenuButtonClass;
	// 팝업 게시판 위젯 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPopupFrame> FrameClass;
	// 생성된 팝업 UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> SettingPageClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HelpPageClasses;
	// 확인 팝업창
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UConfirmPopup> ConfirmPopupClass;

	// 생성된 팝업 UI 인스턴스
	UPROPERTY()
	UPopupFrame* ActivePopUpScreen;

private:
	// 세이브 데이터 여부
	bool bHasSaveData = false;
	// 설정 로드 함수
	void LoadSettings();
	// 버튼 함수들
	UFUNCTION()
	void OnContinueGameClicked();
	UFUNCTION()
	void OnNewStartClicked();
	UFUNCTION()
	void OnHelpClicked();
	UFUNCTION()
	void OnSettingClicked();
	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void OnConfirmedNewStart();
	UFUNCTION()
	void OnConfirmedExit();
};

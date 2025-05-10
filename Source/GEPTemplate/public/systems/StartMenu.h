// StartMenu.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenu.generated.h"

/**
 * 
 */
class UVerticalBox;
class UWBP_MenuButton;

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
	UVerticalBox* BT_List;  // UMG에서 이 변수에 실제 VerticalBox 위젯이 바인딩되어야 함

	// WBP_MenuButton 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> WBP_MenuButtonClass;  // 동적으로 생성할 버튼 클래스

private:
	// 세이브 데이터 확인 여부
	bool bHasSaveData = false;
	// 버튼 함수들
	UFUNCTION()	void OnContinueClicked();
	UFUNCTION()	void OnStartClicked();
	UFUNCTION()	void OnHelpClicked();
	UFUNCTION()	void OnSettingClicked();
	UFUNCTION()	void OnExitClicked();
};

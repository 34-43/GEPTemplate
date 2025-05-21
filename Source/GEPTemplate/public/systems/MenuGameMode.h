// MenuGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

/**
 * 게임 시작 시 전반적인 로딩과 SAVE & LOAD 관리하는 모드베이스
 */
UCLASS()
class GEPTEMPLATE_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMenuGameMode();

	// 게임 화면 전환 함수
	void ShowMainMenu();
	
	// 게임 시작 함수
	void StartGame();

	// 팝업 생성
	UFUNCTION(BlueprintCallable) void ShowSettings();
	UFUNCTION(BlueprintCallable) void ShowHelp();
protected:
	virtual void BeginPlay() override;

	// 메뉴에서 시작할 때 선택한 게임 모드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	TSubclassOf<AGameModeBase> GameModeToStart;

	// 로고 화면 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> StartLogoUI;

	// 메인 메뉴 화면 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> MainMenuUI;

	// 도움말 WBP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HelpPopupClass;
	
	// 설정창 WBP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> SettingsPopupClass;
};

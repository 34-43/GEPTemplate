// StartMenu.cpp


#include "systems/StartMenu.h"
#include "systems/StartMenuBT.h"
#include "systems/MenuGameMode.h"
#include "systems/GameSettingsInstance.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UStartMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// 게임 세이브 확인
	bHasSaveData = UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0);
	// 설정값 로드
	LoadSettings();
	// 버튼을 동적으로 추가하는 함수 호출
	CreateMenuButtons();
}

void UStartMenu::LoadSettings()
{
	UGameSettingsInstance* GameInstance = Cast<UGameSettingsInstance>(GetGameInstance());
	// 설정 세이브 파일 확인 (설정 세이브 파일은 아직 구현 X)
	bool bHasSavedSettings = UGameplayStatics::DoesSaveGameExist(TEXT("SettingSaveSlot"), 0);
}

void UStartMenu::CreateMenuButtons()
{
	// 버튼 메뉴 구현
	if (BT_List && WBP_MenuButtonClass)
	{
		// 버튼 5개 생성
		UStartMenuBT* ContinueButton = CreateWidget<UStartMenuBT>(GetWorld(), WBP_MenuButtonClass);
		UStartMenuBT* StartButton = CreateWidget<UStartMenuBT>(GetWorld(), WBP_MenuButtonClass);
		UStartMenuBT* HelpButton = CreateWidget<UStartMenuBT>(GetWorld(), WBP_MenuButtonClass);
		UStartMenuBT* SettingButton = CreateWidget<UStartMenuBT>(GetWorld(), WBP_MenuButtonClass);
		UStartMenuBT* ExitButton = CreateWidget<UStartMenuBT>(GetWorld(), WBP_MenuButtonClass);
		// 버튼 생성 여부 확인
		if (ContinueButton && StartButton && HelpButton && SettingButton && ExitButton)
		{
			// 버튼을 VerticalBox에 추가
			BT_List->AddChildToVerticalBox(ContinueButton);
			BT_List->AddChildToVerticalBox(StartButton);
			BT_List->AddChildToVerticalBox(HelpButton);
			BT_List->AddChildToVerticalBox(SettingButton);
			BT_List->AddChildToVerticalBox(ExitButton);

			// 버튼 1: 이어하기
			ContinueButton->SetButtonText(FText::FromString(TEXT("이어하기")));
			if (bHasSaveData)
			{
				if (ContinueButton->GetButton())
				{
					ContinueButton->GetButton()->OnClicked.AddDynamic(this, &UStartMenu::OnContinueGameClicked);
				}
			}
			else
			{
				// 세이브 데이터 없을 경우 숨김
				ContinueButton->SetVisibility(ESlateVisibility::Hidden);
			}

			// 버튼 2: 시작하기 or 새로하기
			StartButton->SetButtonText(FText::FromString(TEXT("시작하기")));
			if (StartButton->GetButton())
			{
				StartButton->GetButton()->OnClicked.AddDynamic(this, &UStartMenu::OnNewStartClicked);
			}
			if (bHasSaveData)
			{
				// 세이브 있을 경우 이름 변경
				StartButton->SetButtonText(FText::FromString(TEXT("새로하기")));
			}

			// 버튼 3: 도움말
			HelpButton->SetButtonText(FText::FromString(TEXT("도움말")));
			if (HelpButton->GetButton())
			{
				HelpButton->GetButton()->OnClicked.AddDynamic(this, &UStartMenu::OnHelpClicked);
			}

			// 버튼 4: 설정
			SettingButton->SetButtonText(FText::FromString(TEXT("설정")));
			if (SettingButton->GetButton())
			{
				SettingButton->GetButton()->OnClicked.AddDynamic(this, &UStartMenu::OnSettingClicked);
			}

			// 버튼 5: 종료하기
			ExitButton->SetButtonText(FText::FromString(TEXT("종료하기")));
			if (ExitButton->GetButton())
			{
				ExitButton->GetButton()->OnClicked.AddDynamic(this, &UStartMenu::OnExitClicked);
			}
		}
	}
}

// 이어하기
void UStartMenu::OnContinueGameClicked()
{
	if (UGameSettingsInstance* GameInstance = Cast<UGameSettingsInstance>(GetGameInstance()))
	{
		// 게임 시작
		AMenuGameMode* MenuGameMode = Cast<AMenuGameMode>(UGameplayStatics::GetGameMode(this));
		if (MenuGameMode) MenuGameMode->StartGame();
	}
}

// 시작하기, 새로하기
void UStartMenu::OnNewStartClicked()
{
	if (UGameSettingsInstance* GameInstance = Cast<UGameSettingsInstance>(GetGameInstance()))
	{
		// 세이브 초기화
		if (bHasSaveData) UGameplayStatics::DeleteGameInSlot(TEXT("PlayerSaveSlot"), 0);
		// 게임 시작
		OnContinueGameClicked();
	}
}

void UStartMenu::OnHelpClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;
	AMenuGameMode* GameMode = Cast<AMenuGameMode>(World->GetAuthGameMode());
	if (GameMode) GameMode->ShowHelp();
}

void UStartMenu::OnSettingClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;
	AMenuGameMode* GameMode = Cast<AMenuGameMode>(World->GetAuthGameMode());
	if (GameMode) GameMode->ShowSettings();
}

void UStartMenu::OnExitClicked()
{
	// 게임 종료
	UWorld* currentWorld = GetWorld();
	UKismetSystemLibrary::QuitGame(currentWorld, currentWorld->GetFirstPlayerController(), EQuitPreference::Quit,
	                               false);
}

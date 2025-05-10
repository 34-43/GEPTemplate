// StartMenu.cpp


#include "systems/StartMenu.h"
#include "systems/StartMenuBT.h"
#include "systems/MenuGameMode.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UStartMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼을 동적으로 추가하는 함수 호출
	CreateMenuButtons();
}

void UStartMenu::CreateMenuButtons()
{
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
					ContinueButton->GetButton()->OnClicked.AddDynamic(this, &UStartMenu::OnContinueClicked);
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
				StartButton->GetButton()->OnClicked.AddDynamic(this, &UStartMenu::OnStartClicked);
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

void UStartMenu::OnContinueClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("--이어하기 기능 구현중--"));
}
void UStartMenu::OnStartClicked()
{
	// 현재 게임 모드를 가져옵니다
	AMenuGameMode* MenuGameMode = Cast<AMenuGameMode>(UGameplayStatics::GetGameMode(this));
	if (MenuGameMode)
	{
		// 게임 시작 함수 호출
		MenuGameMode->StartGame();
	}
}
void UStartMenu::OnHelpClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("--도움말 기능 구현중--"));
}
void UStartMenu::OnSettingClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("--설정 기능 구현중--"));
}
void UStartMenu::OnExitClicked()
{
	// 게임 종료
	UWorld* currentWorld = GetWorld();
	UKismetSystemLibrary::QuitGame(currentWorld, currentWorld->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
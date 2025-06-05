// MenuGameMode.cpp

#include "systems/MenuGameMode.h"
#include "systems/SettingsPopup.h"
#include "systems/HelpPopup.h"
#include "systems/GameSettingsInstance.h"
#include "GEPTemplate.h"
#include "GEPTemplateGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AMenuGameMode::AMenuGameMode()
{
	// 기본 생성자
	static ConstructorHelpers::FClassFinder<AGEPTemplateGameModeBase> MainGameModeClass(
		TEXT("/Game/Blueprints/BP_GEPTemplateGameModeBase.BP_GEPTemplateGameModeBase_C"));
	if (MainGameModeClass.Succeeded()) { GameModeToStart = MainGameModeClass.Class; }
}

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameSettingsInstance* Settings = Cast<UGameSettingsInstance>(GetGameInstance());
	if (!Settings) return;
	if (Settings->bLogoNeedToShow)
	{
		Settings->bLogoNeedToShow = false;
		// 로고 화면 띄우기
		if (StartLogoUI)
		{
			UUserWidget* LogoWidget = CreateWidget<UUserWidget>(GetWorld(), StartLogoUI);
			if (LogoWidget)
			{
				LogoWidget->AddToViewport();
			}
		}
	}
	else { ShowMainMenu(); }

	// 입력 처리
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스가 뷰포트 밖으로도 자유롭게 움직이도록 (원하면 변경 가능)
	InputMode.SetHideCursorDuringCapture(false); // 클릭해도 마우스 커서 숨기지 않음
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}

void AMenuGameMode::ShowMainMenu()
{
	// 로고 UI 제거
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UUserWidget::StaticClass(), true);
	for (UUserWidget* Widget : Widgets)
	{
		Widget->RemoveFromParent();
	}

	// 메뉴 UI 띄우기
	if (MainMenuUI)
	{
		UUserWidget* MenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuUI);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
			UGameplayStatics::SetGamePaused(GetWorld(), false); // 재개
		}
	}
}

void AMenuGameMode::StartGame()
{
	if (GameModeToStart)
	{
		// 게임 모드를 설정하고, 새로운 레벨을 로드합니다.
		const FString Options = FString::Printf(TEXT("?game=%s"), *GameModeToStart->GetPathName());
		UGameplayStatics::OpenLevel(GetWorld(), "TemplateMap", true, *Options);
		PRINT_LOG(TEXT("%s"), *Options);
	}

	// 게임 입력 모드 + 마우스 커서 숨기기 (시작할 때 바로 적용)
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		// 클릭 없이 바로 입력 들어가게
		PC->SetInputMode(FInputModeGameOnly());
		// 마우스 커서 숨기기
		PC->bShowMouseCursor = false;
	}
}

void AMenuGameMode::ShowHelp()
{
	UWorld* World = GetWorld();
	if (World && HelpPopupClass)
	{
		UHelpPopup* HelpPopup = CreateWidget<UHelpPopup>(World, HelpPopupClass);
		if (HelpPopup)
		{
			HelpPopup->AddToViewport();
		}
	}
}

void AMenuGameMode::ShowSettings()
{
	UWorld* World = GetWorld();
	if (World && SettingsPopupClass)
	{
		USettingsPopup* SettingsPopup = CreateWidget<USettingsPopup>(World, SettingsPopupClass);
		if (SettingsPopup)
		{
			SettingsPopup->AddToViewport();
		}
	}
}

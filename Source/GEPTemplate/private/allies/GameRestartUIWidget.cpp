// GameRestartUIWidget.cpp

#include "allies/GameRestartUIWidget.h"
#include "Components/Button.h"

void UGameRestartUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReviveButton)
	{
		ReviveButton->OnClicked.AddDynamic(this, &UGameRestartUIWidget::OnReviveClicked);
		
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UGameRestartUIWidget::OnMainMenuClicked);
	}
}

void UGameRestartUIWidget::OnReviveClicked()
{
	OnRevive.Broadcast(); // "부활하기" 버튼 클릭 알림
	RemoveFromParent();
}

void UGameRestartUIWidget::OnMainMenuClicked()
{
	OnMainMenu.Broadcast(); // "시작화면" 버튼 클릭 알림
	RemoveFromParent();
}
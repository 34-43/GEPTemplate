// ConfirmPopup.cpp

#include "systems/ConfirmPopup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UConfirmPopup::NativeConstruct()
{
	Super::NativeConstruct();

	if (YesButton) YesButton->OnClicked.AddDynamic(this, &UConfirmPopup::OnConfirmClicked);

	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UConfirmPopup::OnCloseClicked);
	if (NoButton) NoButton->OnClicked.AddDynamic(this, &UConfirmPopup::OnCloseClicked);
}

void UConfirmPopup::OnConfirmClicked()
{
	OnConfirmed.Broadcast(); // "예" 버튼 클릭 알림
	RemoveFromParent();
}

void UConfirmPopup::OnCloseClicked()
{
	RemoveFromParent();
}

void UConfirmPopup::SetInfoText(const FText& NewText)
{
	if (InfoText)
	{
		InfoText->SetText(NewText);
	}
}
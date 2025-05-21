// PopupFrame.cpp

#include "systems/PopupFrame.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UPopupFrame::NativeConstruct()
{
	Super::NativeConstruct();

	if (PrevButton)
		PrevButton->OnClicked.AddDynamic(this, &UPopupFrame::OnPrevClicked);
	if (NextButton)
		NextButton->OnClicked.AddDynamic(this, &UPopupFrame::OnNextClicked);
	if (CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &UPopupFrame::OnCloseClicked);

	InitializePages();
}

void UPopupFrame::SetPages(const TArray<UUserWidget*>& InPages, const TArray<FText>& InTitles)
{
	Pages = InPages;
	PageTitles = InTitles;

	if (!PageContainer) return;

	PageContainer->ClearChildren();
	for (UWidget* Page : Pages)
	{
		PageContainer->AddChildToVerticalBox(Page);
		Page->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Pages.Num() > 0)
		ShowPage(0);
}


void UPopupFrame::ShowPage(int32 PageIndex)
{
	if (PageIndex < 0 || PageIndex >= Pages.Num()) return;

	for (int32 i = 0; i < Pages.Num(); ++i)
	{
		Pages[i]->SetVisibility(i == PageIndex ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	CurrentPageIndex = PageIndex;

	// 페이지 텍스트 표시
	if (TitleText)
	{
		if (PageTitles.IsValidIndex(PageIndex))
			TitleText->SetText(PageTitles[PageIndex]);
		else
			TitleText->SetText(FText::Format(FText::FromString(TEXT("{0} / {1}")),
			                                 FText::AsNumber(CurrentPageIndex + 1),
			                                 FText::AsNumber(Pages.Num())));
	}

	UpdateNavigationVisibility();
}

void UPopupFrame::UpdateNavigationVisibility()
{
	const int32 NumPages = Pages.Num();

	// 페이지가 2장 미만이면 좌우 버튼 모두 숨김
	if (NumPages < 2)
	{
		if (PrevButton)
			PrevButton->SetVisibility(ESlateVisibility::Collapsed);
		if (NextButton)
			NextButton->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// 2페이지 이상일 때 현재 페이지에 따라 버튼 표시
	if (PrevButton)
	{
		PrevButton->SetVisibility(CurrentPageIndex > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		PrevButton->SetIsEnabled(CurrentPageIndex > 0);
	}

	if (NextButton)
	{
		NextButton->SetVisibility(CurrentPageIndex < NumPages - 1
			                          ? ESlateVisibility::Visible
			                          : ESlateVisibility::Collapsed);
		NextButton->SetIsEnabled(CurrentPageIndex < NumPages - 1);
	}
}


void UPopupFrame::OnPrevClicked()
{
	if (CurrentPageIndex > 0) ShowPage(CurrentPageIndex - 1);
}

void UPopupFrame::OnNextClicked()
{
	if (CurrentPageIndex < Pages.Num() - 1) ShowPage(CurrentPageIndex + 1);
}

void UPopupFrame::OnCloseClicked()
{
	RemoveFromParent();
}

void UPopupFrame::InitializePages()
{
}

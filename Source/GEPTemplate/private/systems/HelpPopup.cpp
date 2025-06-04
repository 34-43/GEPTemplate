// HelpPopup.cpp

#include "systems/HelpPopup.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

void UHelpPopup::InitializePages()
{
	TArray<UUserWidget*> PagesToAdd;

	UWorld* World = GetWorld();
	if (!World) return;

	for (TSubclassOf<UUserWidget> PageClass : PageWidgetClasses)
	{
		if (PageClass)
		{
			UUserWidget* PageWidget = CreateWidget<UUserWidget>(World, PageClass);
			if (PageWidget)
			{
				PagesToAdd.Add(PageWidget);
			}
		}
	}
	TitlesToAdd = {
		FText::FromString(TEXT("기본 조작법")),
		FText::FromString(TEXT("공격하기")),
	};
	SetPages(PagesToAdd, TitlesToAdd);
}
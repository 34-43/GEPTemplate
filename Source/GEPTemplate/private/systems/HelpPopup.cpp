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
		FText::FromString(TEXT("게임 방법 1")),
		FText::FromString(TEXT("게임 방법 2")),
	};
	SetPages(PagesToAdd, TitlesToAdd);
}
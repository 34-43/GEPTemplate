// SettingsPopup.cpp

#include "systems/SettingsPopup.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

void USettingsPopup::InitializePages()
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
		FText::FromString(TEXT("게임 설정"))
	};
	SetPages(PagesToAdd, TitlesToAdd);
}
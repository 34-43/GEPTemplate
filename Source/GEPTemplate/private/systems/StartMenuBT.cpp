// StartMenuBT.cpp


#include "systems/StartMenuBT.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStartMenuBT::SetButtonText(const FText& NewText)
{
	if (Text_Button)
	{
		Text_Button->SetText(NewText);
	}
}
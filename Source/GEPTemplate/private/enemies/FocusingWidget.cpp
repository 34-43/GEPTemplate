#include "enemies/FocusingWidget.h"

#include "Components/Image.h"

void UFocusingWidget::SetFocus(bool Value)
{
	bFocused = Value;
	if (Value)
	{
		FocusDot->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FocusDot->SetVisibility(ESlateVisibility::Hidden);
	}
}

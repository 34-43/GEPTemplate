#include "enemies/FocusedWidget.h"

#include "Components/Image.h"

void UFocusedWidget::SetFocus(const bool Value)
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

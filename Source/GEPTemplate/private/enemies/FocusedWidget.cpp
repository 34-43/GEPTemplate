#include "enemies/FocusedWidget.h"

#include "Components/Image.h"

void UFocusedWidget::NativeConstruct()
{
	if (FocusDot)
	{
		FocusDot->SetVisibility(ESlateVisibility::Visible);
	}
}

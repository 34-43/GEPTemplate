#include "allies/GameAlertUIWidget.h"
#include "Components/TextBlock.h"

void UGameAlertUIWidget::PlayAlert(FString Message, FLinearColor Color)
{
	if (AlertText)
	{
		AlertText->SetText(FText::FromString(Message));
		AlertText->SetColorAndOpacity(FSlateColor(Color));
	}

	if (ShowAlert)
	{
		PlayAnimation(ShowAlert);
	}
}

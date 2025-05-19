#include "allies/PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerHUDWidget::HandleHealthChanged(int32 NewHealth, int32 MaxHealth)
{
	SetHealth(NewHealth / static_cast<float>(MaxHealth));
}

void UPlayerHUDWidget::SetHealth(float Percent)
{
	if (PB_Health)
		PB_Health->SetPercent(Percent);
}

void UPlayerHUDWidget::SetStamina(float Percent)
{
	if (PB_Stamina)
		PB_Stamina->SetPercent(Percent);
}

void UPlayerHUDWidget::SetGold(int32 GoldAmount)
{
	if (Txt_GoldValue)
	{
		FString Text = FString::Printf(TEXT("%d"), GoldAmount);
		Txt_GoldValue->SetText(FText::FromString(Text));
	}
}
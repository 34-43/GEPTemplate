#include "enemies/bosses/BossHUDWidget.h"

#include "Components/ProgressBar.h"

void UBossHUDWidget::HandleHealthChanged(int32 NewHealth, int32 MaxHealth)
{
	if (PB_Health)
	{
		PB_Health->SetPercent(NewHealth / static_cast<float>(MaxHealth));
	}
}

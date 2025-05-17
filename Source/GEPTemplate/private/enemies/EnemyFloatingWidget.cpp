#include "enemies/EnemyFloatingWidget.h"

#include "Components/ProgressBar.h"

void UEnemyFloatingWidget::HandleHealthChanged(int32 NewHealth, int32 MaxHealth)
{
	HealthBar->SetPercent(NewHealth / static_cast<float>(MaxHealth));
}

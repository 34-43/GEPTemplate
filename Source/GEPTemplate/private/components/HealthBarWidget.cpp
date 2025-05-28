// HealthBarWidget.cpp

#include "components/HealthBarWidget.h"
#include "Components/ProgressBar.h"

void UHealthBarWidget::HandleHealthChanged(int32 NewHealth, int32 MaxHealth)
{
	if (MaxHealth <= 0) return; // 0 나누기 방지
	float Percent = static_cast<float>(NewHealth) / static_cast<float>(MaxHealth);
	HealthBar->SetPercent(Percent);
}
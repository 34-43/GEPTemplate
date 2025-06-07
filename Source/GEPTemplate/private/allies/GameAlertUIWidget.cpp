// GameAlertUIWidget.cpp

#include "allies/GameAlertUIWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UGameAlertUIWidget::PlayAlert(FString Message, FLinearColor Color, float Duration)
{
	if (!GetWorld()) return;

	if (AlertText)
	{
		AlertText->SetText(FText::FromString(Message));
		AlertText->SetColorAndOpacity(FSlateColor(Color));
	}

	if (ShowAlert)
	{
		PlayAnimation(ShowAlert);
	}

	GetWorld()->GetTimerManager().ClearTimer(AlertTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(AlertTimerHandle, this, &UGameAlertUIWidget::StartHideAlert, Duration, false);
}

void UGameAlertUIWidget::StartHideAlert()
{
	if (!GetWorld()) return;

	if (HideAlert)
	{
		// 기존 바인딩 해제 (중복 방지)
		if (HideAnimationFinishedDelegate.IsBound())
		{
			UnbindFromAnimationFinished(HideAlert, HideAnimationFinishedDelegate);
		}

		// 델리게이트 생성 및 바인딩
		HideAnimationFinishedDelegate.BindUFunction(this, FName("OnHideAlertFinished"));
		BindToAnimationFinished(HideAlert, HideAnimationFinishedDelegate);

		PlayAnimation(HideAlert);
	}
	else
	{
		RemoveFromParent();
	}
}

void UGameAlertUIWidget::OnHideAlertFinished()
{
	RemoveFromParent();

	// 안전하게 타이머 해제
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AlertTimerHandle);
	}

	// 바인딩 해제
	if (HideAlert && HideAnimationFinishedDelegate.IsBound())
	{
		UnbindFromAnimationFinished(HideAlert, HideAnimationFinishedDelegate);
		HideAnimationFinishedDelegate.Unbind();
	}
}
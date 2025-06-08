// GameAlertUIWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAlertUIWidget.generated.h"

UCLASS()
class GEPTEMPLATE_API UGameAlertUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) class UTextBlock* AlertText;

	UPROPERTY(meta = (BindWidgetAnim), Transient) class UWidgetAnimation* ShowAlert;
	UPROPERTY(meta = (BindWidgetAnim), Transient) class UWidgetAnimation* HideAlert;

	void PlayAlert(FString Message, FLinearColor Color, float Duration = 10.0f);

protected:
	// 애니메이션 종료 콜백용 함수와 델리게이트 변수
	UFUNCTION()	void StartHideAlert();
	UFUNCTION()	void OnHideAlertFinished();

	FTimerHandle AlertTimerHandle;

	// 바인딩 해제용 델리게이트 보관
	FWidgetAnimationDynamicEvent HideAnimationFinishedDelegate;
};
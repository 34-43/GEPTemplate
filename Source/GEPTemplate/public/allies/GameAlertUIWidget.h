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
	
	void PlayAlert(FString Message, FLinearColor Color);
};

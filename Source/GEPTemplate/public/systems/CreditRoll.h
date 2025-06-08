#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditRoll.generated.h"

UCLASS()
class GEPTEMPLATE_API UCreditRoll : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidgetAnim), Transient) class UWidgetAnimation* Roll;
};

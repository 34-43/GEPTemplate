﻿#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FocusedWidget.generated.h"

UCLASS()
class GEPTEMPLATE_API UFocusedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* FocusDot;
};

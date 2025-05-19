#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FocusingWidget.generated.h"

UCLASS()
class GEPTEMPLATE_API UFocusingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* FocusDot;

	void SetFocus(bool Value);

private:
	bool bFocused = false;
};

// SettingsPopup.h

#pragma once

#include "CoreMinimal.h"
#include "PopupFrame.h"
#include "SettingsPopup.generated.h"

class UTextBlock;
class USlider;

UCLASS()
class GEPTEMPLATE_API USettingsPopup : public UPopupFrame
{
	GENERATED_BODY()

protected:
	// 페이지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pages")
	TArray<TSubclassOf<UUserWidget>> PageWidgetClasses;
	// 제목
	UPROPERTY(BlueprintReadOnly, Category = "Pages")
	TArray<FText> TitlesToAdd;
	
	virtual void InitializePages() override;
};
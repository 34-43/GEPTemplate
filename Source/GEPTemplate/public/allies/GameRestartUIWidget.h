// GameRestartUIWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameRestartUIWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReviveConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuConfirmed);

class UButton;

UCLASS()
class GEPTEMPLATE_API UGameRestartUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable) FOnReviveConfirmed OnRevive;
	UPROPERTY(BlueprintAssignable) FOnMainMenuConfirmed OnMainMenu;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION() void OnReviveClicked();
	UFUNCTION() void OnMainMenuClicked();
	
	UPROPERTY(meta = (BindWidget)) UButton* ReviveButton;
	UPROPERTY(meta = (BindWidget)) UButton* MainMenuButton;
};

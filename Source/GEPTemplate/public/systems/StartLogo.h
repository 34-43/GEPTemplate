// StartLogo.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartLogo.generated.h"

/**
 * 
 */
UCLASS()
class GEPTEMPLATE_API UStartLogo : public UUserWidget
{
	GENERATED_BODY()

public:
	// 애니메이션 끝났을 때 호출될 함수
	UFUNCTION(BlueprintCallable) void OnLogoFinished();
};

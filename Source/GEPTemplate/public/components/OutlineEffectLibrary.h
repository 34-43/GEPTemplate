// OutlineEffectLibrary.h

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OutlineEffectLibrary.generated.h"

UCLASS()
class GEPTEMPLATE_API UOutlineEffectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Visual Effects")
	static void SetActorOutline(AActor* Target, bool bEnable);
};

// CoinGenerator.h

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CoinGenerator.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UCoinGenerator : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCoinGenerator();
	UFUNCTION(BlueprintCallable) void GenerateCoins(int32 CoinAmount = 1);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	TSubclassOf<class AItemSample> CoinClass;;
};

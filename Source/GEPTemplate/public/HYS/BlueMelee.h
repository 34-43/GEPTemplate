#pragma once

#include "CoreMinimal.h"
#include "Melee.h"
#include "BlueMelee.generated.h"

UCLASS()
class GEPTEMPLATE_API ABlueMelee : public AMelee
{
	GENERATED_BODY()

public:
	ABlueMelee();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "BlueMelee") int32 InitialBlueMaxHealth = 100;
};

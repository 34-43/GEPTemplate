#pragma once

#include "CoreMinimal.h"
#include "Melee.h"
#include "WhiteMelee.generated.h"

UCLASS()
class GEPTEMPLATE_API AWhiteMelee : public AMelee
{
	GENERATED_BODY()

public:
	AWhiteMelee();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "WhiteMelee") int32 InitialMaxHealth = 100;
};

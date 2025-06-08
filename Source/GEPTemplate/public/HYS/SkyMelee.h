#pragma once

#include "CoreMinimal.h"
#include "Melee.h"
#include "SkyMelee.generated.h"

UCLASS()
class GEPTEMPLATE_API ASkyMelee : public AMelee
{
	GENERATED_BODY()

public:
	ASkyMelee();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

#pragma once

#include "CoreMinimal.h"
#include "Melee.h"
#include "BlackMelee.generated.h"

UCLASS()
class GEPTEMPLATE_API ABlackMelee : public AMelee
{
	GENERATED_BODY()

public:
	ABlackMelee();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

#pragma once

#include "CoreMinimal.h"
#include "Minion.h"
#include "SuperMinion.generated.h"

UCLASS()
class GEPTEMPLATE_API ASuperMinion : public AMinion
{
	GENERATED_BODY()

public:
	ASuperMinion();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

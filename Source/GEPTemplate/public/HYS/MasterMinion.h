#pragma once

#include "CoreMinimal.h"
#include "Minion.h"
#include "MasterMinion.generated.h"

UCLASS()
class GEPTEMPLATE_API AMasterMinion : public AMinion
{
	GENERATED_BODY()

public:
	AMasterMinion();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "MasterMinion") int32 InitialMaxHealth = 100;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "KillBox.generated.h"

UCLASS()
class GEPTEMPLATE_API AKillBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AKillBox();

protected:
	UFUNCTION()
	void Kill(AActor* OverlappedActor, AActor* OtherActor);
	virtual void BeginPlay() override;
};

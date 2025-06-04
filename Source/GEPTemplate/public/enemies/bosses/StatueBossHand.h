#pragma once

#include "CoreMinimal.h"
#include "BaseBossPart.h"
#include "HandAnimInstance.h"
#include "StatueBossHand.generated.h"

UCLASS()
class GEPTEMPLATE_API AStatueBossHand : public ABaseBossPart
{
	GENERATED_BODY()

public:
	AStatueBossHand();

protected:
	virtual void BeginPlay() override;

public:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere) class UPointLightComponent* PointLightC;

	// 핵심 기능
	void PoundAndSpawn(const FOnActionCompleted& OnCompleted);
	void BasicShot(const FOnActionCompleted& OnCompleted);
	void SetHandState(const FRotator& NewHandRotation, const EHandAnimState& NewState);

private:
	// 로직
	UPROPERTY() class UHandAnimInstance* ABPCache;
	FOnActionCompleted OnPatternCompleted;
};

#pragma once

#include "CoreMinimal.h"
#include "BaseBoss.h"
#include "StatueBossHand.h"
#include "StatueBoss.generated.h"

UCLASS()
class GEPTEMPLATE_API AStatueBoss : public ABaseBoss
{
	GENERATED_BODY()

public:
	AStatueBoss();

protected:
	virtual void BeginPlay() override;
	virtual void TriggerBossFight() override;
	void InitializeHub();
	void InitializePatternCycle();
	void ExecuteNextCycle();

public:
	virtual void Tick(float DeltaTime) override;
	void TickHubMovement(float DeltaTime);

	// 컴포넌트
	UPROPERTY(VisibleAnywhere) class UPointLightComponent* PointLightC;
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticEyeC;
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticEyePupilC;
	
	// 팩토리
	UPROPERTY(VisibleAnywhere) TSubclassOf<AStatueBossHand> BossHandF;

	// 파라미터
	UPROPERTY(EditDefaultsOnly) int32 BossHandsNum = 4;
	UPROPERTY(EditDefaultsOnly) TArray<FVector> BossHandsLocations = {
		FVector(1300,1800,400),
		FVector(1000,1600,1800),
		FVector(1300,-1800,400),
		FVector(1000,-1600,1800),
	};
	UPROPERTY(EditDefaultsOnly) FVector FrontLineStartLocation;
	UPROPERTY(EditDefaultsOnly) FVector FrontLineEndLocation;
	
private:
	// 패턴
	void ShockwavePattern();
	void ExplosionPattern();

	// 로직
	int32 PatternBankIndex = 0;
	TArray<int32> PatternBank = {0, 1, 2, 3};
	UPROPERTY() TArray<AStatueBossHand*> BossHands;
	UPROPERTY() TArray<bool> BossHandsControlFlags;
	FTimerHandle StatueBossTimerHandle;
	TArray<FVector> BossHandsWorldLocations;
};

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HandAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EHandAnimState : uint8
{
	Idle = 0,
	Rock = 1,
	Paper = 2
};

UCLASS()
class GEPTEMPLATE_API UHandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int IdleRockPaper;
	void SetAnimState(EHandAnimState AnimState) { IdleRockPaper = static_cast<int>(AnimState); }
};

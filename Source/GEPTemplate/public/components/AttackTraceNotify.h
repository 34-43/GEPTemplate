#pragma once

#include "CoreMinimal.h"
#include "CombatComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AttackTraceNotify.generated.h"

UCLASS()
class GEPTEMPLATE_API UAttackTraceNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override
	{
		if (const AActor* Owner = MeshComp->GetOwner())
		{
			if (auto* Combat = Owner->FindComponentByClass<UCombatComponent>())
			{
				Combat->PerformAttackSweep();
			}
		}
	}
};

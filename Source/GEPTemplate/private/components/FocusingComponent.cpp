#include "components/FocusingComponent.h"

#include "allies/MainCharacter.h"
#include "components/FocusedComponent.h"


UFocusingComponent::UFocusingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UFocusingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFocusingComponent::ScanTargets()
{
	ClearTargets();

	// ScanRange 만큼의 구체를 만들어 오버랩되는 타겟 수집
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const FCollisionShape ScanSphere = FCollisionShape::MakeSphere(ScanRange);
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		OwnerLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		ScanSphere
	);

	// 오버랩 결과 액터들에서 보유한 피집중 컴포넌트를 전부 리스트에 수집
	for (const auto Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (Actor && Actor != GetOwner())
		{
			if (auto FocusedC = Cast<UFocusedComponent>(Actor->GetComponentByClass(UFocusedComponent::StaticClass())))
			{
				FocusedCList.Add(FocusedC);
			}
		}
	}
	SortTargetsByDistance();
}

void UFocusingComponent::CycleTarget()
{
	// 매번 새롭게 스캔하여 리스트를 새로고침한다.
	ScanTargets();

	// 스캔된 대상이 없으면,
	if (FocusedCList.Num() == 0)
	{
		// 최근에 사용한 대상들도 없으면 그냥 종료한다.
		if (!UFocusedComponent::HasRecentlyFocused()) return;
		
		// 있다면 그들의 집중점을 비활성화하고, 사용 이력을 제거한다.
		UFocusedComponent::FlushRecentlyFocusedCList();

		// 주체의 컨트롤러 회전을 기존 방식으로 복구한다.
		if (auto MC = Cast<AMainCharacter>(GetOwner()))
		{
			MC->EndFocusControl();
		}
	}

	// 스캔된 대상(들)이 존재할 때, 그 중 사용 이력이 있는 대상을 현재 리스트에서 제거한다.
	FocusedCList.RemoveAll([](const UFocusedComponent* FocusedC)
	{
		return !FocusedC || FocusedC->IsRecentlyFocused();
	});

	SortTargetsByDistance();
	
	// 남은 완전 새로운 대상 리스트에 하나라도 있는 경우,
	if (FocusedCList.Num() > 0)
	{
		// 그 하나에게 있는 집중점을 활성화하고, 사용 이력을 남기며, 최근에 사용한 대상들의 집중점을 비활성화한다.
		auto NowFocusedC = FocusedCList.Pop();
		NowFocusedC->SetFocus(true);
		
		if (auto MC = Cast<AMainCharacter>(GetOwner()))
		{
			MC->StartFocusControlWithTarget(NowFocusedC);
			// MC->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
	// 스캔된 대상(들)이 존재하면서 동시에 사용이력이 없는 대상이 하나도 없는 경우,
	else
	{
		// 최근에 사용한 대상들의 집중점을 비활성화하고, 사용 이력을 제거한다.
		UFocusedComponent::FlushRecentlyFocusedCList();
		
		// 주체의 컨트롤러 회전을 기존 방식으로 복구한다.
		if (auto MC = Cast<AMainCharacter>(GetOwner()))
		{
			MC->EndFocusControl();
		}
	}
}

void UFocusingComponent::ClearTargets()
{
	FocusedCList.Empty();
}

void UFocusingComponent::SortTargetsByDistance()
{
	// 하나 이하면 정렬의 의미가 없으므로 종료한다.
	if (FocusedCList.Num() <= 1) return;

	// 거리 비교 후 리스트 내부에서 바로 정렬
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	Algo::Sort(FocusedCList, [OwnerLocation](UFocusedComponent* A, UFocusedComponent* B)
	{
		float DistA2OSq = FVector::DistSquared(A->GetOwner()->GetActorLocation(), OwnerLocation);
		float DistB2OSq = FVector::DistSquared(B->GetOwner()->GetActorLocation(), OwnerLocation);
		return DistA2OSq > DistB2OSq;
	});
}

#pragma once

#include "CoreMinimal.h"
#include "FocusedComponent.h"
#include "Components/ActorComponent.h"
#include "FocusingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChangedSignature, UFocusedComponent*, NewTarget);

/**
 * 메인 플레이어 전용으로 구현된 "집중 컴포넌트"로, 소유자의 메서드 호출을 통한 '플레이어'->'피집중 액터'에 관한 동작의 책임을 가진다.
 * @author 34-43
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UFocusingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFocusingComponent();

protected:
	virtual void BeginPlay() override;

public:
	// --- 초기값 ---
	UPROPERTY(EditDefaultsOnly, Category = "Focusing") float ScanRange = 800.0f;

	// --- 이벤트 ---
	UPROPERTY(BlueprintAssignable, Category="Focusing") FOnTargetChangedSignature OnTargetChanged;

	// --- 상호작용 (소유자에서 호출) ---
	UFUNCTION(BlueprintCallable, Category = "Focusing") void ScanTargets();
	UFUNCTION(BlueprintCallable, Category = "Focusing") void CycleTarget();
	UFUNCTION(BlueprintCallable, Category = "Focusing") void ClearTargets();

private:
	// --- 중요 데이터 ---
	UPROPERTY() TArray<UFocusedComponent*> FocusedCList;	//스캔된 타겟 저장하는 배열

	// --- 단순 로직 ---
	void SortTargetsByDistance();
};

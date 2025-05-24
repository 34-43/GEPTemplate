#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FocusedComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyedSignature);

/**
 * 집중 컴포넌트의 스캔 대상이 될 액터에게 붙어 구별하고, 동시에 집중점 위젯을 관리하는 목적인 "피집중 컴포넌트".
 * @author 34-43
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEPTEMPLATE_API UFocusedComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFocusedComponent();
	/**
	 * 소유자의 생성자 안에서, 내부의 위젯을 소유자의 메시 소켓에 부착하기 위한 래퍼 메서드.
	 * 소유자에게 위젯 위치 지정을 의존함.
	 */
	void SetupWidgetAttachment(USceneComponent* InParent, FName InSocketName) const;
	
	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere) class UWidgetComponent* FocusedWidgetC;

	// --- 오버라이드 --- 이 컴포넌트를 수동으로 파괴할 때, 위젯도 함께 파괴되도록 오버라이드
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	// --- 이벤트 ---
	UPROPERTY(BlueprintAssignable) FOnDestroyedSignature OnDestroyed;
	
	// --- 래퍼 --- 런타임에 이 컴포넌트 레벨에서 집중을 제어하기 위한 단순 래퍼
	void SetFocus(bool Value);

	// --- 로직 --- 이 피집중 대상이 집중 주체의 스캔 주기에 한 번 이상 집중되었는 지 기록
	bool bRecentlyFocused;
	void SetRecentlyFocused(const bool Value) { bRecentlyFocused = Value; }
	bool IsRecentlyFocused() const { return bRecentlyFocused; }

	// --- 정적 로직 ---
	static UFocusedComponent* CurrentFocusC;
	static UFocusedComponent* GetCurrentFocusC() { return CurrentFocusC; }
	static TArray<UFocusedComponent*> RecentlyFocusedCList;
	static bool HasRecentlyFocused() { return RecentlyFocusedCList.Num() > 0; }
	static void FlushRecentlyFocusedCList()
	{
		for (const auto RecentlyFocusedC : RecentlyFocusedCList)
		{
			if (RecentlyFocusedC && RecentlyFocusedC->FocusedWidgetC)
			{
				RecentlyFocusedC->SetFocus(false);
			}
			RecentlyFocusedC->SetRecentlyFocused(false);
		}
		RecentlyFocusedCList.Empty();
	}
};

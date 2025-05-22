#include "components/FocusedComponent.h"

#include "Components/WidgetComponent.h"
#include "enemies/FocusedWidget.h"


UFocusedComponent::UFocusedComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 내부 피집중 위젯 설정
	FocusedWidgetC = CreateDefaultSubobject<UWidgetComponent>(TEXT("FocusingWidgetComponent"));
	FocusedWidgetC->SetDrawSize(FVector2D(30, 30));
	FocusedWidgetC->SetRelativeLocation(FVector::ZeroVector);
	FocusedWidgetC->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> FocusedWidgetBP(
		TEXT("/Game/UI/WBP_Focusing.WBP_Focusing_C"));
	if (FocusedWidgetBP.Succeeded()) { FocusedWidgetC->SetWidgetClass(FocusedWidgetBP.Class); }
}

void UFocusedComponent::SetupWidgetAttachment(USceneComponent* InParent, FName InSocketName = NAME_None) const
{
	FocusedWidgetC->SetupAttachment(InParent, InSocketName);
}

void UFocusedComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	FocusedWidgetC->DestroyComponent();
	FocusedWidgetC = nullptr;

	// 중요: 이 컴포넌트와 위젯이 nullptr이 될 때, 참조될 수 있는 위치도 cascade 삭제로 null-safe 처리
	RecentlyFocusedCList.Remove(this);
	if (CurrentFocusC == this)
	{
		CurrentFocusC = nullptr;
	}
}

void UFocusedComponent::SetFocus(const bool Value)
{
	if (const auto FocusedWidget = Cast<UFocusedWidget>(FocusedWidgetC->GetUserWidgetObject()))
	{
		// 집중점을 활성화할 때는 직전 집중 대상을 비활성화하고, 사용 기록도 추가함
		if (Value)
		{
			if (CurrentFocusC && CurrentFocusC->FocusedWidgetC)
			{
				CurrentFocusC->SetFocus(false);
			}
			CurrentFocusC = this;
			SetRecentlyFocused(true);
			RecentlyFocusedCList.Add(this);
		}
		FocusedWidget->SetFocus(Value);
	}
}

// --- 정적 멤버 정의 ---
UFocusedComponent* UFocusedComponent::CurrentFocusC = nullptr;
TArray<UFocusedComponent*> UFocusedComponent::RecentlyFocusedCList;
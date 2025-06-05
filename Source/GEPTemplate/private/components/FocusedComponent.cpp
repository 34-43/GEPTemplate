#include "components/FocusedComponent.h"

#include "allies/MainCharacter.h"
#include "Components/WidgetComponent.h"
#include "enemies/FocusedWidget.h"
#include "components/FocusingComponent.h"


UFocusedComponent::UFocusedComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 내부 피집중 위젯 설정
	FocusedWidgetC = CreateDefaultSubobject<UWidgetComponent>(TEXT("FocusingWidgetComponent"));
	FocusedWidgetC->SetDrawSize(FVector2D(30, 30));
	FocusedWidgetC->SetRelativeLocation(FVector::ZeroVector);
	FocusedWidgetC->SetWidgetSpace(EWidgetSpace::Screen);
	FocusedWidgetC->SetVisibility(false);
	static ConstructorHelpers::FClassFinder<UUserWidget> FocusedWidgetBP(
		TEXT("/Game/UI/WBP_Focusing.WBP_Focusing_C"));
	if (FocusedWidgetBP.Succeeded()) { FocusedWidgetC->SetWidgetClass(FocusedWidgetBP.Class); }
}

void UFocusedComponent::SetupWidgetAttachment(USceneComponent* InParent, FName InSocketName = NAME_None) const
{
	FocusedWidgetC->SetupAttachment(InParent, InSocketName);
}

void UFocusedComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto MC = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		FCCache = Cast<UFocusingComponent>(MC->GetComponentByClass(UFocusingComponent::StaticClass()));
	}
}

void UFocusedComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	OnDestroyed.Broadcast();

	FocusedWidgetC->DestroyComponent();
	FocusedWidgetC = nullptr;
	
	// 중요: 이 컴포넌트와 위젯이 nullptr이 될 때, 참조될 수 있는 위치도 cascade 삭제로 null-safe 처리
	FCCache->RecentlyFocusedCList.Remove(this);
	if (FCCache->CurrentFocusC == this)
	{
		FCCache->CurrentFocusC = nullptr;
	}
}

void UFocusedComponent::SetFocus(const bool Value)
{
	UE_LOG(LogTemp, Warning,
		TEXT("SetFocus: this=%p, FocusedWidgetC=%p, Valid=%d, PendingKill=%d"),
		this,
		FocusedWidgetC,
		FocusedWidgetC && FocusedWidgetC->IsValidLowLevel(),
		FocusedWidgetC && FocusedWidgetC->IsPendingKill()
	);
	
	if (!FocusedWidgetC || !IsValid(FocusedWidgetC)) return;
	
	// 직전 집중 대상을 비활성화
	if (Value && FCCache->CurrentFocusC && FCCache->CurrentFocusC->FocusedWidgetC)
	{
		FCCache->CurrentFocusC->FocusedWidgetC->SetVisibility(false);
	}

	// 집중점 활성화 및 사용 이력 추가
	if (Value)
	{
		FCCache->CurrentFocusC = this;
		SetRecentlyFocused(true);
		FCCache->RecentlyFocusedCList.Add(this);
	}
	
	FocusedWidgetC->SetVisibility(Value);
}

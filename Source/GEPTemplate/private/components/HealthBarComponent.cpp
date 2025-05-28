// HealthBarComponent.cpp

#include "components/HealthBarComponent.h"
#include "components/HealthBarWidget.h"
#include "components/HealthComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

UHealthBarComponent::UHealthBarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// 위젯 클래스 경로 지정
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/UI/WBP_HealthBar"));
	if (WidgetClassFinder.Succeeded())
	{
		HealthBarWidgetClass = WidgetClassFinder.Class;
	}
}

void UHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!HealthBarWidgetClass) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// PlayerController 캐싱
	CachedPC = UGameplayStatics::GetPlayerController(this, 0);
	// WidgetComponent 생성 및 설정
	WidgetComponent = NewObject<UWidgetComponent>(Owner);
	if (!WidgetComponent) return;
}

void UHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType,
										FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 프레임 스킵
	if (++FrameCounter % 3 == 0) UpdateWidget();
}

void UHealthBarComponent::UpdateWidget()
{
	if (!CachedPC || !CachedPC->PlayerCameraManager) return;

	const FVector CameraLocation = CachedPC->PlayerCameraManager->GetCameraLocation();
	const FVector WidgetLoc = WidgetComponent->GetComponentLocation();

	const float Distance = FVector::DistSquared(CameraLocation, WidgetLoc);
	constexpr float FadeStart = 800.0f * 800.0f;
	constexpr float FadeEnd = 1200.0f * 1200.0f;
	const float OpacityFactor = 1.0f - FMath::GetMappedRangeValueClamped(
		FVector2D(FadeStart, FadeEnd), FVector2D(0.0f, 1.0f), Distance);

	UUserWidget* LocalWidget = WidgetComponent->GetUserWidgetObject();
	if (LocalWidget)
	{
		LocalWidget->SetRenderOpacity(OpacityFactor);
	}
	WidgetComponent->SetWorldRotation((CameraLocation - WidgetComponent->GetComponentLocation()).Rotation());
}

void UHealthBarComponent::ShowHealthBar()
{
	WidgetComponent->SetVisibility(true);
}

void UHealthBarComponent::HideHealthBar()
{
	WidgetComponent->SetVisibility(false);
}
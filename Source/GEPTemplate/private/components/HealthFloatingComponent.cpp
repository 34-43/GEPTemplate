// HealthFloatingComponent.cpp

#include "components/HealthFloatingComponent.h"
#include "components/HealthBarWidget.h"
#include "components/HealthComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

UHealthFloatingComponent::UHealthFloatingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 위젯 클래스 경로 지정
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/UI/WBP_HealthBar"));
	if (WidgetClassFinder.Succeeded())
	{
		HealthBarWidgetClass = WidgetClassFinder.Class;
	}
	// WidgetComponent 생성 및 기본 설정
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatingWidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(100, 8));
	WidgetComponent->SetRelativeLocation(FVector(0, 0, 110));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
}


// Called when the game starts
void UHealthFloatingComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthFloatingComponent: Owner is null."));
		return;
	}
	if (!HealthBarWidgetClass) return;
	
	// 위젯 컴포넌트를 소유자의 루트 컴포넌트에 붙임
	if (Owner->GetRootComponent())
	{
		WidgetComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		// 위젯 클래스 설정 후 초기화
		WidgetComponent->SetWidgetClass(HealthBarWidgetClass);
		WidgetComponent->InitWidget();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthFloatingComponent: Owner root component is null."));
	}
	
	// HealthComponent 캐싱
	HealthC = Owner->FindComponentByClass<UHealthComponent>();
	if (!HealthC)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthFloatingComponent: UHealthComponent not found on %s"), *Owner->GetName());
		return;
	}
	// 위젯에서 UHealthBarWidget 타입으로 캐스팅 후 체력 변경 이벤트 바인딩
	if (UUserWidget* UserWidget = WidgetComponent->GetUserWidgetObject())
	{
		UHealthBarWidget* HealthBarWidget = Cast<UHealthBarWidget>(UserWidget);
		if (HealthBarWidget)
		{
			HealthC->OnHealthChanged.AddDynamic(HealthBarWidget, &UHealthBarWidget::HandleHealthChanged);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HealthFloatingComponent: Failed to cast UserWidget to HealthBarWidget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthFloatingComponent: Failed to get UserWidgetObject"));
	}
	
	// PlayerController 캐싱
	CachedPC = UGameplayStatics::GetPlayerController(this, 0);
	if (!CachedPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthFloatingComponent: Failed to get PlayerController for %s"), *Owner->GetName());
		return;
	}
}


// Called every frame
void UHealthFloatingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 프레임 스킵
	if (++FrameCounter % 3 == 0) UpdateWidget();
}

// 위젯 위치, 투명도, 방향 갱신
void UHealthFloatingComponent::UpdateWidget()
{
	if (!CachedPC || !CachedPC->PlayerCameraManager) return;

	const FVector CameraLocation = CachedPC->PlayerCameraManager->GetCameraLocation();
	const FVector WidgetLoc = WidgetComponent->GetComponentLocation();
	// 카메라와 위젯 거리 제곱 계산
	const float Distance = FVector::DistSquared(CameraLocation, WidgetLoc);
	constexpr float FadeStart = 800.0f * 800.0f;
	constexpr float FadeEnd = 1200.0f * 1200.0f;
	// 거리 기준으로 투명도 보간 (FadeStart~FadeEnd 구간)
	const float OpacityFactor = 1.0f - FMath::GetMappedRangeValueClamped(
		FVector2D(FadeStart, FadeEnd), FVector2D(0.0f, 1.0f), Distance);

	UUserWidget* LocalWidget = WidgetComponent->GetUserWidgetObject();
	if (LocalWidget)
	{
		LocalWidget->SetRenderOpacity(OpacityFactor);
	}
	// 위젯이 카메라를 바라보도록 회전 설정
	WidgetComponent->SetWorldRotation((CameraLocation - WidgetComponent->GetComponentLocation()).Rotation());
}

// 위젯 보이기
void UHealthFloatingComponent::ShowHealthBar()
{
	WidgetComponent->SetVisibility(true);
}

// 위젯 숨기기
void UHealthFloatingComponent::HideHealthBar()
{
	WidgetComponent->SetVisibility(false);
}

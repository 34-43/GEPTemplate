// BgmTrackTrigger.cpp

#include "objects/BgmTrackTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "systems/BgmPlayer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DrawDebugHelpers.h"

ABgmTrackTrigger::ABgmTrackTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
}

// void ABgmTrackTrigger::OnConstruction(const FTransform& Transform)
// {
// 	Super::OnConstruction(Transform);
//
// #if WITH_EDITOR
// 	// TriggerRadius 크기의 구를 에디터에서 표시
// 	DrawDebugSphere(
// 		GetWorld(),
// 		GetActorLocation(),
// 		TriggerRadius,
// 		16,                      // 세그먼트 수 (둥글게)
// 		FColor::Green,
// 		false,                   // 지속 표시 (false면 잠깐만 보여짐)
// 		-1.0f,                   // 시간 (음수는 무제한)
// 		0,
// 		2.f                     // 선 두께
// 	);
// #endif
// }

void ABgmTrackTrigger::BeginPlay()
{
	Super::BeginPlay();

	CachedPlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ABgmTrackTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 틱 스킵
	FrameCounter = (FrameCounter + 1) % FrameCounterRate;
	if (FrameCounter != 0) return;

	if (!bIsBgmPlayerWidgetValid) FindBgmPlayerWidget();
	if (!BgmPlayerWidget) return;

	const float Distance = GetDistanceToPlayer();
	const bool bNowInRange = Distance <= TriggerRadius;

	if (bNowInRange && !bIsPlayerInRange)
	{
		// 플레이어가 진입했을 때
		BgmPlayerWidget->SetTrack(TrackIndex);
		bIsPlayerInRange = true;
	}
	else if (!bNowInRange && bIsPlayerInRange)
	{
		// 플레이어가 범위를 벗어났을 때
		bIsPlayerInRange = false;
	}
}

float ABgmTrackTrigger::GetDistanceToPlayer() const
{
	if (!CachedPlayerActor) return FLT_MAX;
	return FVector::Dist(GetActorLocation(), CachedPlayerActor->GetActorLocation());
}

void ABgmTrackTrigger::FindBgmPlayerWidget()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UBgmPlayer::StaticClass(), false);

	if (FoundWidgets.Num() > 0)
	{
		BgmPlayerWidget = Cast<UBgmPlayer>(FoundWidgets[0]);
		if (BgmPlayerWidget)
		{
			bIsBgmPlayerWidgetValid = true;
		}
	}
}

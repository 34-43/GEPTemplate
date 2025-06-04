// BgmTrackTrigger.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BgmTrackTrigger.generated.h"

class UBgmPlayer;

UCLASS()
class GEPTEMPLATE_API ABgmTrackTrigger : public AActor
{
	GENERATED_BODY()

public:
	ABgmTrackTrigger();

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// BGM 변경 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BGM")
	float TriggerRadius = 400.f;

	// 변경할 BGM 트랙 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BGM")
	int32 TrackIndex = 0;

	// 플레이어 캐릭터 참조 (캐싱용)
	UPROPERTY()
	AActor* CachedPlayerActor = nullptr;

	// BGM 플레이어 위젯 참조
	UPROPERTY()
	UBgmPlayer* BgmPlayerWidget = nullptr;

	// 플레이어와 트리거 사이 거리 계산
	float GetDistanceToPlayer() const;

	// BGM 위젯을 화면에서 찾아 참조 시도
	void FindBgmPlayerWidget();

private:
	// 위젯 참조 성공 여부 (중복 탐색 방지)
	bool bIsBgmPlayerWidgetValid = false;
	// 유저 범위 확인 저장
	bool bIsPlayerInRange = false;
	// 틱 카운트
	int32 FrameCounter = 0;
	int32 FrameCounterRate = 16;
};

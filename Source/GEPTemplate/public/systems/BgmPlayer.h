// BgmPlayer.h
// 배경음악 플레이어 UI 위젯 클래스
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BgmPlayer.generated.h"

class UTextBlock;

UCLASS()
class GEPTEMPLATE_API UBgmPlayer : public UUserWidget
{
	GENERATED_BODY()

public:
	// 지정한 인덱스에 해당하는 트랙으로 변경
	// 인덱스가 유효하지 않으면 아무 동작하지 않음
	UFUNCTION(BlueprintCallable) void SetTrack(int32 Index);
	int32 GetTrack() const { return CurrentTrackIndex; }

protected:
	// 현재 재생 중인 오디오 컴포넌트
	// 페이드인/페이드아웃 재생 제어에 사용
	UPROPERTY()
	UAudioComponent* AudioComponent = nullptr;

	// 재생 가능한 배경음악 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Music", meta=(AllowPrivateAccess=true))
	TArray<USoundBase*> MusicArray;

	// 트랙 번호를 표시하는 UI 텍스트 블록 (UMG 위젯 바인딩)
	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* TrackIndex;

	// 페이드인/페이드아웃에 사용할 시간 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Music", meta=(AllowPrivateAccess=true))
	float FadeDuration = 1.0f;

	UFUNCTION()	void OnTrackFinished();
	
	// 새로운 트랙을 실제로 재생하는 함수
	void PlayNewTrack(USoundBase* NewTrack);

	// 위젯이 파괴될 때 호출되는 함수, UUserWidget에서 오버라이드
	virtual void NativeDestruct() override;
	
private:
	// 현재 선택된 트랙 인덱스
	// 0부터 시작, -1 등 유효하지 않은 값은 사용하지 않음
	int32 CurrentTrackIndex = -1;
};

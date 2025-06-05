// SoundSettings.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSettings.generated.h"

class UTextBlock;
class USlider;
class UCheckBox;

UCLASS()
class GEPTEMPLATE_API UGameSettings : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	// 배경음악 슬라이더 및 텍스트
	UPROPERTY(meta = (BindWidget)) USlider* SliderBgmVolume;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TextBgmValue;
	// 효과음 슬라이더 및 텍스트
	UPROPERTY(meta = (BindWidget)) USlider* SliderSfxVolume;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TextSfxValue;
	// 음성 슬라이더 및 텍스트
	UPROPERTY(meta = (BindWidget)) USlider* SliderVoxVolume;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TextVoxValue;
	// 로고 플레이 설정
	UPROPERTY(meta = (BindWidget)) UCheckBox* LogoCheckBox;

private:
	// 슬라이더 값 변경 이벤트 핸들러
	UFUNCTION()	void OnBGMVolumeChanged(float NewValue);
	UFUNCTION()	void OnSFXVolumeChanged(float NewValue);
	UFUNCTION()	void OnVOXVolumeChanged(float NewValue);
	UFUNCTION()	void OnLogoCheckChanged(bool bIsChecked);
};

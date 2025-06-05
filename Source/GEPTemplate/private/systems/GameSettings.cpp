// SoundSettings.cpp

#include "systems/GameSettings.h"
#include "systems/GameSettingsInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"

void UGameSettings::NativeConstruct()
{
	Super::NativeConstruct();

	UGameSettingsInstance* Settings = Cast<UGameSettingsInstance>(GetGameInstance());
	if (!Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("SoundSettings: GameInstance 캐스트 실패!"));
		return;
	}

	SliderBgmVolume->SetValue(Settings->BGMVolume);
	TextBgmValue->SetText(
		FText::Format(NSLOCTEXT("SoundSettings", "BGMPercent", "{0}%"), FMath::RoundToInt(Settings->BGMVolume * 100)));

	SliderSfxVolume->SetValue(Settings->SFXVolume);
	TextSfxValue->SetText(
		FText::Format(NSLOCTEXT("SoundSettings", "SFXPercent", "{0}%"), FMath::RoundToInt(Settings->SFXVolume * 100)));

	SliderVoxVolume->SetValue(Settings->VOXVolume);
	TextVoxValue->SetText(
		FText::Format(NSLOCTEXT("SoundSettings", "VOXPercent", "{0}%"), FMath::RoundToInt(Settings->VOXVolume * 100)));

	LogoCheckBox->SetIsChecked(Settings->bLogoVisible);

	// 슬라이더 이벤트 연결
	SliderBgmVolume->OnValueChanged.AddDynamic(this, &UGameSettings::OnBGMVolumeChanged);
	SliderSfxVolume->OnValueChanged.AddDynamic(this, &UGameSettings::OnSFXVolumeChanged);
	SliderVoxVolume->OnValueChanged.AddDynamic(this, &UGameSettings::OnVOXVolumeChanged);
	LogoCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameSettings::OnLogoCheckChanged);
}

void UGameSettings::OnBGMVolumeChanged(float NewValue)
{
	if (UGameSettingsInstance* Settings = Cast<UGameSettingsInstance>(GetGameInstance()))
	{
		// 게임 세팅 인스턴스에 값 저장
		Settings->BGMVolume = NewValue;
		Settings->ApplySettings(); // 볼륨 실제 적용 함수 호출

		// UI 텍스트 업데이트 (0~150%)
		TextBgmValue->SetText(
			FText::Format(
				NSLOCTEXT("SoundSettings", "BGMPercent", "{0}%"), FMath::RoundToInt(Settings->BGMVolume * 100)));
	}
}

void UGameSettings::OnSFXVolumeChanged(float NewValue)
{
	if (UGameSettingsInstance* Settings = Cast<UGameSettingsInstance>(GetGameInstance()))
	{
		Settings->SFXVolume = NewValue;
		Settings->ApplySettings();

		TextSfxValue->SetText(
			FText::Format(
				NSLOCTEXT("SoundSettings", "SFXPercent", "{0}%"), FMath::RoundToInt(Settings->SFXVolume * 100)));
	}
}

void UGameSettings::OnVOXVolumeChanged(float NewValue)
{
	if (UGameSettingsInstance* Settings = Cast<UGameSettingsInstance>(GetGameInstance()))
	{
		Settings->VOXVolume = NewValue;
		Settings->ApplySettings();

		TextVoxValue->SetText(
			FText::Format(
				NSLOCTEXT("SoundSettings", "VOXPercent", "{0}%"), FMath::RoundToInt(Settings->VOXVolume * 100)));
	}
}

void UGameSettings::OnLogoCheckChanged(bool bIsChecked)
{
	if (UGameSettingsInstance* Settings = Cast<UGameSettingsInstance>(GetGameInstance()))
	{
		Settings->bLogoVisible = bIsChecked;
		Settings->ApplySettings();
	}
}

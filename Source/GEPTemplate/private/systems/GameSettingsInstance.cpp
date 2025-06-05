// GameSettingsInstance.cpp

#include "systems/GameSettingsInstance.h"
#include "Kismet/GameplayStatics.h"
#include "allies/MainCharacter.h"
#include "systems/GEPSaveGame.h"  // SaveGame 클래스

void UGameSettingsInstance::Init()
{
	Super::Init();
	// 게임 시작 시 설정 로드 및 적용
	LoadGameSettings();
	bLogoNeedToShow = bLogoVisible;
	ApplySettings();
}

void UGameSettingsInstance::SetBGMVolume(float NewVolume)
{
	BGMVolume = FMath::Clamp(NewVolume, 0.0f, 1.5f);
	ApplySettings();
}

void UGameSettingsInstance::SetSFXVolume(float NewVolume)
{
	SFXVolume = FMath::Clamp(NewVolume, 0.0f, 1.5f);
	ApplySettings();
}

void UGameSettingsInstance::SetVOXVolume(float NewVolume)
{
	VOXVolume = FMath::Clamp(NewVolume, 0.0f, 1.5f);
	ApplySettings();
}

void UGameSettingsInstance::ApplySettings()
{
	// 사운드 클래스에 실시간 적용
	if (BGMSoundClass) BGMSoundClass->Properties.Volume = BGMVolume;
	if (SFXSoundClass) SFXSoundClass->Properties.Volume = SFXVolume;
	if (VOXSoundClass) VOXSoundClass->Properties.Volume = VOXVolume;
	SaveGameSettings();
}

void UGameSettingsInstance::SaveGameSettings()
{
	// 설정 전용 슬롯에 저장
	UGEPSaveGame* SettingsSave = Cast<UGEPSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UGEPSaveGame::StaticClass()));

	if (SettingsSave)
	{
		SettingsSave->SavedBGMVolume = BGMVolume;
		SettingsSave->SavedSFXVolume = SFXVolume;
		SettingsSave->SavedVOXVolume = VOXVolume;
		SettingsSave->SavedLogoVisible = bLogoVisible;
		UGameplayStatics::SaveGameToSlot(SettingsSave, TEXT("SettingsSlot"), 0);
	}
}

void UGameSettingsInstance::LoadGameSettings()
{
	// 설정 전용 슬롯에서 불러오기
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		UGEPSaveGame* Loaded = Cast<UGEPSaveGame>(
			UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));

		if (Loaded)
		{
			BGMVolume = Loaded->SavedBGMVolume;
			SFXVolume = Loaded->SavedSFXVolume;
			VOXVolume = Loaded->SavedVOXVolume;
			bLogoVisible = Loaded->SavedLogoVisible;
		}
	}
}

void UGameSettingsInstance::SavePlayerData(AMainCharacter* Player)
{
	if (!Player) return;

	// 플레이어 전용 슬롯에 저장
	UGEPSaveGame* SaveGameInstance = Cast<UGEPSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UGEPSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->PlayerData = Player->GetSaveData();
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0);
	}
}

void UGameSettingsInstance::LoadPlayerData(AMainCharacter* Player)
{
	if (!Player) return;

	if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
	{
		UGEPSaveGame* Loaded = Cast<UGEPSaveGame>(
			UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));
		if (Loaded)
		{
			Player->LoadFromSaveData(Loaded->PlayerData);
		}
	}
}

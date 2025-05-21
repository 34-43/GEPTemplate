// GameSettingsInstance.cpp


#include "systems/GameSettingsInstance.h"
#include "Kismet/GameplayStatics.h"
#include "allies/MainCharacter.h"
#include "systems/GEPSaveGame.h"  // SaveGame 클래스 헤더 포함

void UGameSettingsInstance::SetBGMVolume(float NewVolume)
{
	BGMVolume = FMath::Clamp(NewVolume, 0.0f, 1.5f);
}

void UGameSettingsInstance::SetSFXVolume(float NewVolume)
{
	SFXVolume = FMath::Clamp(NewVolume, 0.0f, 1.5f);
}

void UGameSettingsInstance::SavePlayerData(AMainCharacter* Player)
{
	if (!Player) return;

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
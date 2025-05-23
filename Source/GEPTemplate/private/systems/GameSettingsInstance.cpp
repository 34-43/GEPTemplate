// GameSettingsInstance.cpp


#include "systems/GameSettingsInstance.h"
#include "Kismet/GameplayStatics.h"
#include "allies/MainCharacter.h"
#include "systems/GEPSaveGame.h"  // SaveGame 클래스 헤더 포함

void UGameSettingsInstance::Init()
{
	// 특정 정적 멤버가 PIE(플레이 인 에디터) 모드 재시작 시 초기화되지 않고 남아 참조할 수 없는 값이 발생하는 문제에 대한 처리
	UFocusedComponent::CurrentFocusC = nullptr;
	UFocusedComponent::RecentlyFocusedCList.Empty();
}

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
		//SaveGameInstance->PlayerData = Player->GetSaveData();
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
			//Player->LoadFromSaveData(Loaded->PlayerData);
		}
	}
}
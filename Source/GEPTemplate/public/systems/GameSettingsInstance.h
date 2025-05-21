// GameSettingsInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameSettingsInstance.generated.h"

/**
 * 배경음, 효과음, 세이브 파일 불러오기 여부 저장하는 인스턴스
 */
UCLASS()
class GEPTEMPLATE_API UGameSettingsInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category="Audio") float BGMVolume = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category="Audio")	float SFXVolume = 1.0f;

	UFUNCTION(BlueprintCallable, Category="Audio")	void SetBGMVolume(float NewVolume);
	UFUNCTION(BlueprintCallable, Category="Audio")	void SetSFXVolume(float NewVolume);

	UPROPERTY(BlueprintReadOnly, Category="GameSave")	bool bIsContinue = false;

	// 저장/로드 함수
	UFUNCTION(BlueprintCallable, Category="GameSave")
	void SavePlayerData(class AMainCharacter* Player);

	UFUNCTION(BlueprintCallable, Category="GameSave")
	void LoadPlayerData(class AMainCharacter* Player);
};
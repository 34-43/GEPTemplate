// GameSettingsInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameSettingsInstance.generated.h"

class AMainCharacter;
/**
 * 배경음, 효과음, 세이브 파일 불러오기 여부 저장하는 인스턴스
 */
UCLASS()
class GEPTEMPLATE_API UGameSettingsInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// 초기화 시 호출
	virtual void Init() override;

	// 볼륨 설정
	void SetBGMVolume(float NewVolume);
	void SetSFXVolume(float NewVolume);
	void SetVOXVolume(float NewVolume);
	
	// 설정 저장, 로드
	void SaveGameSettings();
	void LoadGameSettings();
	
	// 플레이어 저장, 로드 함수
	void SavePlayerData(AMainCharacter* Player);  // 플레이어 데이터 저장 (PlayerSlot)
	void LoadPlayerData(AMainCharacter* Player);  // 플레이어 데이터 불러오기

	// 현재 볼륨을 실제 적용
	void ApplySettings();

	// 사운드 클래스 참조 (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")	USoundClass* BGMSoundClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")	USoundClass* SFXSoundClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")	USoundClass* VOXSoundClass;

	// 볼륨 설정 값
	UPROPERTY()	float BGMVolume = 1.0f;
	UPROPERTY()	float SFXVolume = 1.0f;
	UPROPERTY()	float VOXVolume = 1.0f;

	// 로고 게임 당 한번만 띄우도록
	UPROPERTY() bool bLogoNeedToShow = true;

	// 로고 설정 값
	UPROPERTY() bool bLogoVisible = true;
};

﻿// GEPSaveGame.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GEPSaveGame.generated.h"

/**
 * 데이터 컨테이너 역할입니다
 * 저장 기능은 Instance에 있습니다.
 */
USTRUCT(BlueprintType) struct FPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY()	FVector Location;
	UPROPERTY()	FRotator Rotation;
	UPROPERTY()	FRotator CameraRotation;

	UPROPERTY() int32 Gold;
	UPROPERTY() int32 BgmTrack;
	// 아이템 수량 배열 저장
	UPROPERTY()	TArray<int32> ItemCounts;
	FPlayerSaveData()
	{
		ItemCounts.Init(0, 2); // 크기 2, 0으로 초기화
	}
};

UCLASS()
class GEPTEMPLATE_API UGEPSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// 게임 진행용
	UPROPERTY()	FPlayerSaveData PlayerData;
	// 설정용
	UPROPERTY()	float SavedBGMVolume = 1.0f;
	UPROPERTY()	float SavedSFXVolume = 1.0f;
	UPROPERTY()	float SavedVOXVolume = 1.0f;
	UPROPERTY()	bool SavedLogoVisible = true;
};
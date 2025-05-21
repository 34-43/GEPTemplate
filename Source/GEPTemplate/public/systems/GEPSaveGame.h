// GEPSaveGame.h

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

	UPROPERTY()	FVector Position;
	UPROPERTY()	float HP;
	UPROPERTY()	float Stamina;
	UPROPERTY() int32 Gold;
};

UCLASS()
class GEPTEMPLATE_API UGEPSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FPlayerSaveData PlayerData;
};

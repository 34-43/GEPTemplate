// CoinGenerator.cpp

#include "components/CoinGenerator.h"
#include "objects/ItemSample.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UCoinGenerator::UCoinGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
	// 기본 생성 아이템 - 코인
	static ConstructorHelpers::FClassFinder<AItemSample> CoinBPClass(TEXT("/Game/Blueprints/BP_GoldCoin.BP_GoldCoin_C"));
	if (CoinBPClass.Succeeded())
	{
		CoinClass = CoinBPClass.Class;
	}
}

void UCoinGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void UCoinGenerator::GenerateCoins(int32 CoinAmount)
{
	if (!GetWorld() || !IsValid(CoinClass) || CoinAmount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters or state in GenerateCoins."));
		return;
	}

	const FVector Center = GetComponentLocation();
	const float Radius = 150.f;

	for (int32 i = 0; i < CoinAmount; ++i)
	{
		float Angle = FMath::RandRange(0.f, 2 * PI);
		float Distance = FMath::RandRange(0.f, Radius);
		FVector SpawnOffset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Distance;
		FVector SpawnLocation = Center + SpawnOffset;
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetOwner() ? GetOwner()->GetInstigator() : nullptr;

		AItemSample* SpawnedCoin = GetWorld()->SpawnActor<AItemSample>(CoinClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (!SpawnedCoin)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn coin at location: %s"), *SpawnLocation.ToString());
		}
	}
}
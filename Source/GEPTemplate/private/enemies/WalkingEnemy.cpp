#include "enemies/WalkingEnemy.h"

#include "components/StateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AWalkingEnemy::AWalkingEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;	// 메시를 이동방향으로 계속 갱신하는 기능 사용
	
	// Set State Component
	StateC = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
}

void AWalkingEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWalkingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWalkingEnemy::TickAI(float DeltaTime)
{
	// Super::TickAI(DeltaTime);
	
	
}

void AWalkingEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


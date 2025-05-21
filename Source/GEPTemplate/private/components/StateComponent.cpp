#include "components/StateComponent.h"

#include "allies/MainCharacter.h"
#include "enemies/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"

UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass());
	target = Cast<AMainCharacter>(actor);
	me = Cast<ABaseEnemy>(GetOwner());
}


void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	}
}

void UStateComponent::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > idleDelayTime)
	{
		mState = EEnemyState::Move;
		currentTime = 0;
	}
}

void UStateComponent::MoveState()
{
	FVector destination = target->GetActorLocation();
	FVector dir = destination - me->GetActorLocation();
	float distance = dir.Size();

	if (distance < 200.0f)
	{
		return;
	}
	
	me->AddMovementInput(dir.GetSafeNormal());
}


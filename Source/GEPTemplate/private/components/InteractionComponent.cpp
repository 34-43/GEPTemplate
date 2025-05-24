// InteractionComponent.cpp

#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Blueprint/UserWidget.h"
#include "components/OutlineEffectLibrary.h"

// 상호작용 액터
#include "objects/WaterDispenser.h"

AActor* UInteractionComponent::GetOwnerActor() const
{
	return OwnerActor ? OwnerActor : GetOwner();
}

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();
	PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!InteractionWidgetClass) return;

	InteractionWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), InteractionWidgetClass);

	if (!InteractionWidgetInstance) return;

	InteractionWidgetInstance->AddToViewport();
	InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
}

void UInteractionComponent::ShowHighlight(bool bShow)
{
	UOutlineEffectLibrary::SetActorOutline(GetOwner(), bShow);
}

void UInteractionComponent::ShowUI(bool bShow)
{
	if (!InteractionWidgetInstance) return;
	InteractionWidgetInstance->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}


void UInteractionComponent::TryInteract()
{
	if (!OwnerActor) return;
	
	if (auto WaterDispenser = Cast<AWaterDispenser>(OwnerActor))
	{
		AActor* Player = PlayerRef;
		WaterDispenser->Interact(Player);
	}
}

bool UInteractionComponent::IsInRange() const
{
	return GetDistanceToPlayer() < InteractRange;
}

float UInteractionComponent::GetDistanceToPlayer() const
{
	if (!OwnerActor || !PlayerRef) return FLT_MAX;
	return FVector::Dist(PlayerRef->GetActorLocation(), OwnerActor->GetActorLocation());
}
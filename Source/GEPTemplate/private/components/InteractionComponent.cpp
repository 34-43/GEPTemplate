// InteractionComponent.cpp

#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "components/OutlineEffectLibrary.h"

// 상호작용 액터
#include "objects/WaterDispenser.h"
#include "objects/ExplosiveBarrel.h"
#include "objects/SavePoint.h"

UInteractionComponent::UInteractionComponent()
{
	// 기본값 하드코딩 - 위젯 클래스 로드 (경로는 프로젝트에 맞게 조정)
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/UI/WBP_ActionHotkey"));
	if (WidgetClassFinder.Succeeded())
	{
		InteractionWidgetClass = WidgetClassFinder.Class;
	}
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

	// 프로그레스 이미지 캐스팅
	UImage* FoundProgressImage = Cast<UImage>(InteractionWidgetInstance->GetWidgetFromName(TEXT("ProgressImage")));
	if (FoundProgressImage)
	{
		UMaterialInterface* BaseMaterial = Cast<UMaterialInterface>(FoundProgressImage->Brush.GetResourceObject());
		if (BaseMaterial)
		{
			ProgressMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			FoundProgressImage->SetBrushFromMaterial(ProgressMaterial);
			ProgressImage = FoundProgressImage;
		}
	}
	SetProgress(0.f);
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

void UInteractionComponent::SetProgress(float Value)
{
	if (ProgressMaterial)
	{
		ProgressMaterial->SetScalarParameterValue(TEXT("Percent"), Value);
	}
}

void UInteractionComponent::TriggerInteraction()
{
	if (!OwnerActor || !PlayerRef) return;
	// 액터를 추가하세요
	if (auto WaterDispenser = Cast<AWaterDispenser>(OwnerActor))
	{
		AActor* Player = PlayerRef;
		WaterDispenser->Interact(Player);
	}
	else if (auto ExplosiveBarrel = Cast<AExplosiveBarrel>(OwnerActor))
	{
		AActor* Player = PlayerRef;
		ExplosiveBarrel->Interact(Player);
	}
	else if (auto SavePoint = Cast<ASavePoint>(OwnerActor))
	{
		AActor* Player = PlayerRef;
		SavePoint->Interact(Player);
	}
	else
	{
		// fallback
		UE_LOG(LogTemp, Warning, TEXT("Unknown interactable actor: %s"), *OwnerActor->GetName());
	}
}

bool UInteractionComponent::IsInRange() const
{
	return GetDistanceToPlayer() < InteractRange;
}

float UInteractionComponent::GetDistanceToPlayer() const
{
	if (!OwnerActor) return FLT_MAX;
	if (!PlayerRef)
	{
		// PlayerRef가 없다면 GetWorld()에서 플레이어 폰을 다시 가져오는 시도
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!PlayerPawn) return FLT_MAX;
		return FVector::Dist(OwnerActor->GetActorLocation(), PlayerPawn->GetActorLocation());
	}
	return FVector::Dist(OwnerActor->GetActorLocation(), PlayerRef->GetActorLocation());
}

#include "allies/MinimapCaptureActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AMinimapCaptureActor::AMinimapCaptureActor()
{
	PrimaryActorTick.bCanEverTick = true;

    CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
    RootComponent = CaptureComponent;

    // 위에서 아래로 보는 시점
    CaptureComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
    CaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
    CaptureComponent->OrthoWidth = 2048.f; // 범위 조절
    CaptureComponent->bCaptureEveryFrame = true;
}

void AMinimapCaptureActor::BeginPlay()
{
	Super::BeginPlay();

	if (RenderTarget)
	{
		CaptureComponent->TextureTarget = RenderTarget;
	}
}

void AMinimapCaptureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		SetActorLocation(Player->GetActorLocation() + FVector(0.f, 0.f, 850.f));
		SetActorRotation(FRotator(-90.f, 0.f, 0.f)); // 고정된 시점
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "allies/MinimapCaptureActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
AMinimapCaptureActor::AMinimapCaptureActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = false;

    CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
    RootComponent = CaptureComponent;

    // 위에서 아래로 보는 시점
    CaptureComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
    CaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
    CaptureComponent->OrthoWidth = 2048.f; // 범위 조절
    CaptureComponent->bCaptureEveryFrame = true;
}

// Called when the game starts or when spawned
void AMinimapCaptureActor::BeginPlay()
{
	Super::BeginPlay();

	if (RenderTarget)
	{
		CaptureComponent->TextureTarget = RenderTarget;
	}
}

// Called every frame
void AMinimapCaptureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


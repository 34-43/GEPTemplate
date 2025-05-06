// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapCaptureActor.generated.h"

UCLASS()
class GEPTEMPLATE_API AMinimapCaptureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinimapCaptureActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    class USceneCaptureComponent2D* CaptureComponent;

    UPROPERTY(EditAnywhere, Category = "MiniMap")
    class UTextureRenderTarget2D* RenderTarget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapCaptureActor.generated.h"

UCLASS()
class GEPTEMPLATE_API AMinimapCaptureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMinimapCaptureActor();

protected:
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    class USceneCaptureComponent2D* CaptureComponent;

    UPROPERTY(EditAnywhere, Category = "MiniMap")
    class UTextureRenderTarget2D* RenderTarget;

public:	
	virtual void Tick(float DeltaTime) override;

};

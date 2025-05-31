// SavePoint.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SavePoint.generated.h"

class UStaticMeshComponent;
class UInteractionComponent;
class UAudioComponent;
class USoundCue;

UCLASS()
class GEPTEMPLATE_API ASavePoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASavePoint();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 정수기 메쉬 (Static Mesh)
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;
	// 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere) UInteractionComponent* InteractC;
	// 오디오 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound") UAudioComponent* AudioC;
	// 사운드 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")	USoundCue* SaveSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")	float SoundRange;

public:
	// 상호작용 함수 (InteractionComponent에서 호출)
	void Interact(AActor* Caller);
	void PlaySound();
	void StopSound();
};

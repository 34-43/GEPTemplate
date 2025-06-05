// VendingMachine.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VendingMachine.generated.h"

class UStaticMeshComponent;
class UInteractionComponent;

UCLASS()
class GEPTEMPLATE_API AVendingMachine : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVendingMachine();

	// 상호작용 컴포넌트, UI에서 접근가능
	UPROPERTY(VisibleAnywhere) UInteractionComponent* InteractC;

protected:
	virtual void BeginPlay() override;

	// 자판기 메쉬 (Static Mesh)
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;

	// 상점 UI
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<UUserWidget> ShopUIClass;
	UPROPERTY() UUserWidget* ShopUIInstance;
	
public:
	// 상호작용 함수 (InteractionComponent에서 호출)
	void Interact(AActor* Caller);
};
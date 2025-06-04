// VendingMachine.h

#include "objects/VendingMachine.h"
#include "components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "systems/VendingMachineUI.h"

AVendingMachine::AVendingMachine()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("Object"));
	RootComponent = Mesh;

	// 상호작용 컴포넌트 설정
	InteractC = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void AVendingMachine::BeginPlay()
{
	Super::BeginPlay();
}

void AVendingMachine::Interact(AActor* Caller)
{
	// UE_LOG(LogTemp, Log, TEXT("%s interacted with %s!"), *Caller->GetName(), *GetName());
	if (!Caller) return;
	// 상호작용 종료
	InteractC->SetPower(false);

	if (!ShopUIClass) return;

	APlayerController* PC = Cast<APlayerController>(Caller->GetInstigatorController());
	if (!PC) return;

	if (!ShopUIInstance)
	{
		ShopUIInstance = CreateWidget<UUserWidget>(PC, ShopUIClass);
		if (UVendingMachineUI* VMUI = Cast<UVendingMachineUI>(ShopUIInstance))
		{
			VMUI->OwningVendingMachine = this;
		}
	}
	if (ShopUIInstance)
	{
		ShopUIInstance->AddToViewport();
		PC->SetInputMode(FInputModeUIOnly());
		PC->bShowMouseCursor = true;
		//UGameplayStatics::SetGamePaused(GetWorld(), true);
		PC->FlushPressedKeys();
	}
}
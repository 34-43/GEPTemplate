// VendingMachineUI.cpp

#include "systems/VendingMachineUI.h"
#include "systems/GameSettingsInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "objects/VendingMachine.h"
#include "components/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "allies/MainCharacter.h"

void UVendingMachineUI::NativeConstruct()
{
	Super::NativeConstruct();

	UGameSettingsInstance* Settings = Cast<UGameSettingsInstance>(GetGameInstance());
	if (!Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("VendingMachine: GameInstance 캐스트 실패!"));
		return;
	}
	// 버튼 이벤트 연결 - 기존 바인딩 제거 후 연결
	if (CloseButton)
	{
		CloseButton->OnClicked.Clear();
		CloseButton->OnClicked.AddDynamic(this, &UVendingMachineUI::CloseShop);
	}
	if (BuyButton0)
	{
		BuyButton0->OnClicked.Clear();
		BuyButton0->OnClicked.AddDynamic(this, &UVendingMachineUI::BuyItem0);
	}
	if (BuyButton1)
	{
		BuyButton1->OnClicked.Clear();
		BuyButton1->OnClicked.AddDynamic(this, &UVendingMachineUI::BuyItem1);
	}
	// 상점 로드
	LoadShop();
}

void UVendingMachineUI::LoadShop()
{
	if (!PlayerRef)
	{
		APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		PlayerRef = Cast<AMainCharacter>(Pawn);
		if (!PlayerRef)
		{
			UE_LOG(LogTemp, Warning, TEXT("VendingMachineUI: 플레이어 캐스팅 실패!"));
		}
	}
	// 코인 불러오기
	Coin = PlayerRef->GetGold();
	CoinText->SetText(FText::AsNumber(Coin));
	// 가격표 불러오기
	PriceText0->SetText(FText::AsNumber(Price[0]));
	PriceText1->SetText(FText::AsNumber(Price[1]));
}

void UVendingMachineUI::CloseShop()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->FlushPressedKeys();
		PC->bShowMouseCursor = false;
		//UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	if (OwningVendingMachine && OwningVendingMachine->InteractC)
	{
		OwningVendingMachine->InteractC->SetPower(true);
	}

	RemoveFromParent();
}

void UVendingMachineUI::BuyItem(int32 ItemCode)
{
	if (!PlayerRef) return;
	if (ItemCode < 0 || ItemCode >= 2 ) return;
	if (Coin >= Price[ItemCode])
	{
		PlayerRef->ManageGold(-Price[ItemCode]);
		PlayerRef->AddItem(ItemCode);
		Coin -= Price[ItemCode];
		CoinText->SetText(FText::AsNumber(Coin));
	}
}
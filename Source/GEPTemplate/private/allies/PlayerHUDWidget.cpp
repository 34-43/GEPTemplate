#include "allies/PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Image_Progress_1)
	{
		UMaterialInterface* BaseMat = Cast<UMaterialInterface>(Image_Progress_1->Brush.GetResourceObject());
		if (BaseMat)
		{
			ProgressMaterial1 = UMaterialInstanceDynamic::Create(BaseMat, this);
			Image_Progress_1->SetBrushFromMaterial(ProgressMaterial1);
		}
	}

	if (Image_Progress_2)
	{
		UMaterialInterface* BaseMat = Cast<UMaterialInterface>(Image_Progress_2->Brush.GetResourceObject());
		if (BaseMat)
		{
			ProgressMaterial2 = UMaterialInstanceDynamic::Create(BaseMat, this);
			Image_Progress_2->SetBrushFromMaterial(ProgressMaterial2);
		}
	}

	SetItem1Progress(0);
	SetItem2Progress(0);
}

void UPlayerHUDWidget::HandleHealthChanged(int32 NewHealth, int32 MaxHealth)
{
	SetHealth(NewHealth / static_cast<float>(MaxHealth));
}

void UPlayerHUDWidget::HandleStaminaChanged(float NewStamina, float MaxStamina)
{
	SetStamina(NewStamina / (MaxStamina));
}

void UPlayerHUDWidget::HandleBoostChanged(float NewBoost, float MaxBoost)
{
	SetBoost(NewBoost / (MaxBoost));
}

void UPlayerHUDWidget::SetHealth(float Percent)
{
	if (PB_Health)
		PB_Health->SetPercent(Percent);
}

void UPlayerHUDWidget::SetStamina(float Percent)
{
	if (PB_Stamina)
		PB_Stamina->SetPercent(Percent);
}

void UPlayerHUDWidget::SetBoost(float Percent)
{
	if (PB_Boost)
	{
		if (FMath::IsNearlyZero(Percent, 0.01f))
		{
			BoostOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			BoostOverlay->SetVisibility(ESlateVisibility::Visible);
			PB_Boost->SetPercent(Percent);
		}
	}
}

void UPlayerHUDWidget::SetGold(int32 GoldAmount)
{
	if (Txt_GoldValue)
	{
		FString Text = FString::Printf(TEXT("%d"), GoldAmount);
		Txt_GoldValue->SetText(FText::FromString(Text));
	}
}

void UPlayerHUDWidget::SetItem1(int32 ItemAmount)
{
	if (Txt_Item_1)
	{
		FString Text = FString::Printf(TEXT("%d"), ItemAmount);
		Txt_Item_1->SetText(FText::FromString(Text));
	}
	SetItem1Opacity(ItemAmount > 0);
}

void UPlayerHUDWidget::SetItem2(int32 ItemAmount)
{
	if (Txt_Item_2)
	{
		FString Text = FString::Printf(TEXT("%d"), ItemAmount);
		Txt_Item_2->SetText(FText::FromString(Text));
	}
	SetItem2Opacity(ItemAmount > 0);
}

void UPlayerHUDWidget::SetItem1Opacity(bool bActive)
{
	if (Img_Item_1)
	{
		Img_Item_1->SetRenderOpacity(bActive ? 1.0f : 0.4f);
	}
}

void UPlayerHUDWidget::SetItem2Opacity(bool bActive)
{
	if (Img_Item_2)
	{
		Img_Item_2->SetRenderOpacity(bActive ? 1.0f : 0.4f);
	}
}

void UPlayerHUDWidget::SetItem1Progress(float Value)
{
	if (ProgressMaterial1)
	{
		if (FMath::IsNearlyZero(Value, 0.01f))
		{
			Value = 0.f;
		}
		ProgressMaterial1->SetScalarParameterValue(TEXT("Percent"), Value);
	}
}

void UPlayerHUDWidget::SetItem2Progress(float Value)
{
	if (ProgressMaterial2)
	{
		if (FMath::IsNearlyZero(Value, 0.01f))
		{
			Value = 0.f;
		}
		ProgressMaterial2->SetScalarParameterValue(TEXT("Percent"), Value);
	}
}

void UPlayerHUDWidget::UpdateItemHUD(int32 Index, int32 ItemAmount, float ProgressValue, bool bInCooldown)
{
	const bool bUsable = (ItemAmount > 0) && !bInCooldown;

	if (Index == 0)
	{
		if (Txt_Item_1)
			Txt_Item_1->SetText(FText::AsNumber(ItemAmount));

		if (Img_Item_1)
			Img_Item_1->SetRenderOpacity(bUsable ? 1.0f : 0.4f);

		if (ProgressMaterial1)
			ProgressMaterial1->SetScalarParameterValue(TEXT("Percent"), ProgressValue);
	}
	else if (Index == 1)
	{
		if (Txt_Item_2)
			Txt_Item_2->SetText(FText::AsNumber(ItemAmount));

		if (Img_Item_2)
			Img_Item_2->SetRenderOpacity(bUsable ? 1.0f : 0.4f);

		if (ProgressMaterial2)
			ProgressMaterial2->SetScalarParameterValue(TEXT("Percent"), ProgressValue);
	}
}

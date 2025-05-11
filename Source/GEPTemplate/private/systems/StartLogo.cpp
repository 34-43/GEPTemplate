// StartLogo.cpp

#include "systems/StartLogo.h"
#include "systems/MenuGameMode.h"
#include "Kismet/GameplayStatics.h"

void UStartLogo::OnLogoFinished()
{
	AMenuGameMode* Gm = Cast<AMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (Gm)
	{
		Gm->ShowMainMenu();
	}
}
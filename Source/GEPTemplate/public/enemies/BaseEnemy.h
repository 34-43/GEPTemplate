#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class GEPTEMPLATE_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	UFUNCTION()
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void TickRenderWidget(APlayerController* PC);

	// 컴포넌트
	UPROPERTY(VisibleAnywhere) class UCombatComponent* CombatC;
	UPROPERTY(VisibleAnywhere) class UHealthComponent* HealthC;
	UPROPERTY(VisibleAnywhere) class UWidgetComponent* FloatingWidgetC;

	// 팩토리
	UPROPERTY(VisibleAnywhere) UParticleSystem* DamagedFxF;
	UPROPERTY(VisibleAnywhere) USoundBase* DamagedSfxF;

private:
	// 델리게이트 핸들러
	UFUNCTION() void HandleDeath();
	UFUNCTION() void HandleDamaged();
	
	// 로직
	UPROPERTY() APlayerController* MainPlayerController;
	void RagDollImpulse();
};

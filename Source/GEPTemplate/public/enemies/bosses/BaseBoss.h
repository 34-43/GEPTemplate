#pragma once

#include "CoreMinimal.h"
#include "BaseBossPart.h"
#include "GameFramework/Character.h"
#include "BaseBoss.generated.h"

/**
 * 보스 공통 기능만 정의하기 위한 클래스.
 * 본체와 파츠가 존재할 때, 본체는 이 클래스를 상속하여 구현함.
 * 공통 컴포넌트: 체력 컴포넌트, 피집중 컴포넌트
 * 공통 위젯: 보스 피통 HUD, 승리 메시지 UI(+호출 메서드)
 *
 * @author 34-43
 */
UCLASS()
class GEPTEMPLATE_API ABaseBoss : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseBoss();

protected:
	virtual void BeginPlay() override;
	void InitializeBossHUD();
	void InitializeGameAlert();
	void InitializeCreditRoll();

public:
	virtual void Tick(float DeltaTime) override;

	// 컴포넌트
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* StaticMeshC;
	UPROPERTY(VisibleAnywhere) class UHealthComponent* TotalHealthC;
	UPROPERTY(VisibleAnywhere) class UFocusedComponent* FocusedC;

	// 위젯
	UPROPERTY(VisibleAnywhere) TSubclassOf<UUserWidget> BossHUD_W;
	UPROPERTY(VisibleAnywhere) TSubclassOf<UUserWidget> GameAlertUI_W;
	UPROPERTY(VisibleAnywhere) TSubclassOf<UUserWidget> CreditRoll_W;
	
	// 상호작용
	void ShowVictoryUI() const;
	UFUNCTION(BlueprintCallable) virtual void TriggerBossFight();

	// 팩토리
	UPROPERTY() UParticleSystem* ExplosionFxF;
	
protected:
	// 캐시
	UPROPERTY() class AMainCharacter* Player;
	
private:
	// 델리게이트 핸들러
	UFUNCTION() virtual void HandleDeath();
	UFUNCTION() virtual void HandleDamaged();
	
	// 위젯
	UPROPERTY() UUserWidget* BossHUDWidget;
	UPROPERTY() UUserWidget* GameAlertUIWidget;
	UPROPERTY() UUserWidget* CreditRollWidget;

	// 로직
	FTimerHandle BaseBossTimerHandle;
};

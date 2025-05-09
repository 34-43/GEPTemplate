#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class GEPTEMPLATE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Camera) class USpringArmComponent* SpringArmC;
	UPROPERTY(VisibleAnywhere, Category = Camera) class UCameraComponent* CameraC;
	UPROPERTY(VisibleAnywhere, Category = GunMesh) USkeletalMeshComponent* GunMeshC;
	UPROPERTY(VisibleAnywhere, Category = GunMesh) UStaticMeshComponent* SniperMeshC;
	UPROPERTY(VisibleAnywhere, Category = Combat) class UCombatComponent* CombatC;

	// 팩토리
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory) TSubclassOf<class ABaseBullet> BulletF;
	UPROPERTY(EditDefaultsOnly, Category = SniperUI) TSubclassOf<UUserWidget> SniperUiF;
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect) UParticleSystem* BulletEffectF;

	// 위젯
	// 미니맵 위젯 변수
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<class UUserWidget> MiniMapWidgetClass;
	// 유저 상태 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UUserWidget> PlayerHUDClass;
	// 개임 알림 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UUserWidget> GameAlertUIClass;
	
	// 델리게이트
	void Turn(float Value);
	void LookUp(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void InputJump();
	void InputFire();
	void InputChangeGrenadeGun();
	void InputChangeSniperGun();
	void InputSniperAim();
	// 상태 변경 함수
	void ManageHealth(float Damage); // 체력 관리
	void ManageStamina(float Amount); // 스태미너 관리
	void RecoverStamina(); // 스태미너 회복
	void ManageGold(int32 Amount); // 골드 관리
	// 유다이 알림 함수
	void ShowDeathUI();

private:
	// 로직
	FVector2D InputDirection = FVector2D::ZeroVector;
	bool bUsingGrenadeGun = true;
	bool bSniperAim = false;
	UUserWidget* _sniperUI;

	// 미니맵 변수
    UUserWidget* MiniMapWidget;
	void TickMovement();
	void InitializeMiniMap();

	// 유저 상태 변수
	UUserWidget* PlayerHUDWidget;
	// 체력
	float MaxHealth = 100.f;
	float CurrentHealth = 50.f;
	// 스태미너
	float MaxStamina = 100.f;
	float CurrentStamina = 80.f;
	float StaminaRecoveryRate = 5.f; // 초당 스태미너 회복량
	FTimerHandle StaminaRecoveryTimer;
	// 골드
	int32 CurrentGold = 0;
	void InitializePlayerHUD();

	// 게임 알림 변수
	UUserWidget* GameAlertUIWidget;
	void InitializeGameAlert();

};

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
	// UPROPERTY(VisibleAnywhere, Category = GunMesh) USkeletalMeshComponent* GunMeshC;
	// UPROPERTY(VisibleAnywhere, Category = GunMesh) UStaticMeshComponent* SniperMeshC;
	UPROPERTY(VisibleAnywhere, Category = Combat) class UCombatComponent* CombatC;
	UPROPERTY(VisibleAnywhere, Category = Health) class UHealthComponent* HealthC;
	UPROPERTY(VisibleAnywhere, Category = Wield) UStaticMeshComponent* BatMeshC;

	// 팩토리
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory) TSubclassOf<class ABaseBullet> BulletF;
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect) UParticleSystem* BulletEffectF;

	// 위젯
	// UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UUserWidget> SniperUI_W;
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<UUserWidget> MiniMapW;// 미니맵 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UUserWidget> PlayerHUD_W;// 유저 상태 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UUserWidget> GameAlertUI_W;// 개임 알림 위젯 변수
	
	// 델리게이트
	void Turn(float Value);
	void LookUp(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void InputJump();
	// void InputFire();
	// void InputChangeGrenadeGun();
	// void InputChangeSniperGun();
	// void InputSniperAim();
	
	// 상태 변경
	void ManageHealth(float Damage); // 체력 관리
	void ManageStamina(float Amount); // 스태미너 관리
	void RecoverStamina(); // 스태미너 회복
	void ManageGold(int32 Amount); // 골드 관리
	
	// UI 표시
	void ShowDeathUI();
	
private:
	// 로직
	FVector2D InputDirection = FVector2D::ZeroVector;
	// bool bUsingGrenadeGun = true;
	// bool bSniperAim = false;

	// 틱 프로시저
	void TickMovement();
	
	// 위젯
	void InitializeMiniMap();
	void InitializePlayerHUD();
	void InitializeGameAlert();
	// UUserWidget* _sniperUI;
	UUserWidget* MiniMapWidget;// 미니맵 변수
	UUserWidget* PlayerHUDWidget;// 유저 상태 변수
	UUserWidget* GameAlertUIWidget;// 게임 알림 변수

	// 스탯
	float MaxHealth = 100.f;
	float CurrentHealth = 50.f;
	float MaxStamina = 100.f;
	float CurrentStamina = 80.f;
	float StaminaRecoveryRate = 5.f; // 초당 스태미너 회복량
	FTimerHandle StaminaRecoveryTimer;// 스태미너 타이머
	int32 CurrentGold = 0;
};

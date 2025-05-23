#pragma once

#include "CoreMinimal.h"
#include "components/FocusedComponent.h"
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

	void Roll();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Camera) class USpringArmComponent* SpringArmC;
	UPROPERTY(VisibleAnywhere, Category = Camera) class UCameraComponent* CameraC;
	// UPROPERTY(VisibleAnywhere, Category = GunMesh) USkeletalMeshComponent* GunMeshC;
	// UPROPERTY(VisibleAnywhere, Category = GunMesh) UStaticMeshComponent* SniperMeshC;
	UPROPERTY(VisibleAnywhere, Category = Wield) UStaticMeshComponent* BatMeshC;
	UPROPERTY(VisibleAnywhere, Category = Combat) class UCombatComponent* CombatC;
	UPROPERTY(VisibleAnywhere, Category = Focusing) class UFocusingComponent* FocusingC;
	UPROPERTY(VisibleAnywhere, Category = Health) class UHealthComponent* HealthC;
	UPROPERTY(VisibleAnywhere, Category = Stamina) class UStaminaComponent* StaminaC;

	// 팩토리
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory) TSubclassOf<class ABaseBullet> BulletF;
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect) UParticleSystem* BulletEffectF;
	UPROPERTY(VisibleAnywhere) UParticleSystem* DamagedFxF;
	UPROPERTY(VisibleAnywhere) UParticleSystem* ParriedFxF;
	UPROPERTY(VisibleAnywhere) USoundBase* DamagedSfxF;
	UPROPERTY(VisibleAnywhere) USoundBase* ParriedSfxF;

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

	// 상호작용
	void SetIgnoreMove(const bool Value) { bIgnoreMove = Value; }
	void SetOverMove(const bool Value) { bOverMove = Value; }
	void SetOverMoveParams(const FVector NewDirection, const float Scale)
	{
		OverMoveDirection = NewDirection;
		OverMoveScale = Scale;
	}
	void StartFocusControlWithTarget(UFocusedComponent* Target);
	UFUNCTION() void EndFocusControl();
	
	// 상태 변경
	void ManageGold(int32 Amount); // 골드 관리
	
	// UI 표시
	void ShowDeathUI();
	
private:
	// 델리게이트 핸들러
	UFUNCTION() void HandleDeath();
	UFUNCTION() void HandleDamaged();
	UFUNCTION() void HandleParried();
	UFUNCTION() void HandleStaggered();
	
	// 로직
	FVector2D InputDirection = FVector2D::ZeroVector;
	// bool bUsingGrenadeGun = true;
	// bool bSniperAim = false;
	bool bIgnoreMove = false;
	bool bOverMove = false;
	FVector OverMoveDirection = FVector::ZeroVector;
	float OverMoveScale = 1.0f;
	bool bOverControl = false;
	UPROPERTY() UFocusedComponent* TargetFocusedC;

	// 틱 프로시저
	void TickMovement(float DeltaTime);
	void TickStamina(float DeltaTime); // stamina natural recovery
	void TickFocusControl(float DeltaTime);

	// 위젯
	void InitializeMiniMap();
	void InitializePlayerHUD();
	void InitializeGameAlert();
	// UUserWidget* _sniperUI;
	UUserWidget* MiniMapWidget;// 미니맵 변수
	UUserWidget* PlayerHUDWidget;// 유저 상태 변수
	UUserWidget* GameAlertUIWidget;// 게임 알림 변수

	// 스탯
	float StaminaRecoveryRate = 5.f; // 초당 스태미너 회복량
	int32 CurrentGold = 0;
};

inline void AMainCharacter::StartFocusControlWithTarget(UFocusedComponent* Target)
{
	TargetFocusedC = Target;
	bOverControl = true;

	Target->OnDestroyed.AddDynamic(this, &AMainCharacter::EndFocusControl);
}

inline void AMainCharacter::EndFocusControl()
{
	TargetFocusedC = nullptr;
	bOverControl = false;
}
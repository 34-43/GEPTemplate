#pragma once

#include <d2d1helper.h>

#include "CoreMinimal.h"
#include "components/FocusedComponent.h"
#include "GameFramework/Character.h"
#include "systems/GEPSaveGame.h"
#include "MainCharacter.generated.h"

class UInteractionComponent;
class UBgmPlayer;
class UGameRestartUIWidget;

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
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<UUserWidget> MiniMapW;// 미니맵 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UUserWidget> PlayerHUD_W;// 유저 상태 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UUserWidget> GameAlertUI_W;// 개임 알림 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UUserWidget> BGMPlayer_W;// BMG 재생 위젯 변수
	UPROPERTY(EditAnywhere, Category="UI") TSubclassOf<UGameRestartUIWidget> Restart_W;// 부활 버튼 위젯 변수

	// 델리게이트
	void Turn(float Value);
	void LookUp(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void InputJump();
	void StartInteract();
	void CancelInteract();

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
	bool ManageGold(int32 Amount); // 골드 관리
	int32 GetGold() const { return CurrentGold; } // 골드 정보
	
	// UI 표시
	void ShowDeathUI();

	// 저장 불러오기
	FPlayerSaveData GetSaveData() const;
	void LoadFromSaveData(const FPlayerSaveData& Data);

	// 아이템 배열, 사용, 획득
	TArray<int32> ItemCounts = {0, 0};
	void AddItem(int32 ItemCode);
	void UseItem(int32 ItemCode);
	
private:
	// 델리게이트 핸들러
	UFUNCTION() void HandleDeath();
	UFUNCTION() void HandleDamaged();
	UFUNCTION() void HandleParried();
	UFUNCTION() void HandleStaggered();
	UFUNCTION() void HandleRestart();
	UFUNCTION() void HandleGoStartMenu();

	// 상호작용 물체 처리
	void HandleInteractionHoldTick(float DeltaTime);
	void UpdateInteractionFocus();
	UPROPERTY() UInteractionComponent* CurrentInteractionComponent = nullptr;
	float MaxDistance = 350.f; // 탐지 최대 거리
	int FrameCounter = 0;
	bool bIsInteracting = false;
	float InteractHoldTime = 0.f;
	float RequiredHoldTime = 0.f;
	
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
	void InitializeBgmPlayer();
	// UUserWidget* _sniperUI;
	UPROPERTY() UUserWidget* MiniMapWidget;// 미니맵 변수
	UPROPERTY() UUserWidget* PlayerHUDWidget;// 유저 상태 변수
	UPROPERTY() UUserWidget* GameAlertUIWidget;// 게임 알림 변수
	UPROPERTY() UBgmPlayer* BgmPlayerWidget;// BGM 플레이어 변수

	// 스탯
	float StaminaRecoveryRate = 5.f; // 초당 스태미너 회복량
	int32 CurrentGold = 0;
	// 부스트 상태
	float SuperBoostTime = 0.f;
	float MaxBoostTime = 30.f;

	// 아이템 사용 누르기
	// 눌린 아이템 번호 (0 = 없음, 1 = 아이템1, 2 = 아이템2)
	int32 PressedItemIndex = 0;
	// 누르기 시작한 시간 저장
	float PressStartTime = 0.f;
	const float HoldThreshold = 1.0f;
	void OnItem1Pressed();
	void OnItem2Pressed();
	void OnItemReleased();
	void UpdateItemUsage(float DeltaTime);
	bool bItemInCooldown[2] = { false, false };
	float ItemCooldownTime[2] = { 0.0f, 0.0f };
	const float CooldownDuration = 2.0f;
	void ShowRestartPopup();

	// 사망 함수
	bool bIsDead = false;
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
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
	UPROPERTY(VisibleAnywhere, Category = Wield) UStaticMeshComponent* BatMeshC;

	// 팩토리
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory) TSubclassOf<class ABaseBullet> BulletF;
	UPROPERTY(EditDefaultsOnly, Category = SniperUI) TSubclassOf<UUserWidget> SniperUiF;
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect) UParticleSystem* BulletEffectF;
	
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

private:
	// 로직
	FVector2D InputDirection = FVector2D::ZeroVector;
	// bool bUsingGrenadeGun = true;
	// bool bSniperAim = false;
	// UUserWidget* _sniperUI;
	void TickMovement();
};

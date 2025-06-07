#include "HYS/MeleeCombatComponent.h"

#include "GEPTemplate.h"
#include "components/HealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYS/Melee.h"

UMeleeCombatComponent::UMeleeCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CombatState = ECombatState::IdleMoving;

	// 공격 몽타주 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeAttackA_M_Obj(
		TEXT("/Game/HYS/Melee/Montage/Melee_AttackA_Montage.Melee_AttackA_Montage"));
	if (MeleeAttackA_M_Obj.Succeeded()) { MeleeAttackA_M = MeleeAttackA_M_Obj.Object; }

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeAttackB_M_Obj(
		TEXT("/Game/HYS/Melee/Montage/Melee_AttackB_Montage.Melee_AttackB_Montage"));
	if (MeleeAttackB_M_Obj.Succeeded()) { MeleeAttackB_M = MeleeAttackB_M_Obj.Object; }

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeAttackC_M_Obj(
		TEXT("/Game/HYS/Melee/Montage/Melee_AttackC_Montage.Melee_AttackC_Montage"));
	if (MeleeAttackC_M_Obj.Succeeded()) { MeleeAttackA_M = MeleeAttackC_M_Obj.Object; }

	// 패링 몽타주 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeParry_M_Obj(
		TEXT("/Game/HYS/Melee/Montage/Melee_Parry_Montage.Melee_Parry_Montage"));
	if (MeleeParry_M_Obj.Succeeded()) { MeleeParry_M = MeleeParry_M_Obj.Object; }

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeParrySuccess_M_Obj(
		TEXT("/Game/HYS/Melee/Montage/Melee_Parry_Success_Montage.Melee_Parry_Success_Montage"));
	if (MeleeParrySuccess_M_Obj.Succeeded()) { MeleeParrySuccess_M = MeleeParrySuccess_M_Obj.Object; }

	// 죽음 몽타주 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeDeathA_M_Obj(
		TEXT("/Game/HYS/Melee/Montage/Melee_DeathA_Montage.Melee_DeathA_Montage"));
	if (MeleeDeathA_M_Obj.Succeeded()) { MeleeDeathA_M = MeleeDeathA_M_Obj.Object; }
}


void UMeleeCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		if (UAnimInstance* AnimInst = OwnerChar->GetMesh()->GetAnimInstance())
		{
			AnimInst->OnMontageEnded.AddDynamic(this, &UCombatComponent::OnMontageEnded);
		}
	}
}

void UMeleeCombatComponent::Attack()
{
	// AnimInst 레퍼런스 준비
	UAnimInstance* AnimInst = nullptr;
	if (const ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		AnimInst = Char->GetMesh()->GetAnimInstance();
	}

	if (CombatState == ECombatState::IdleMoving)
	{
		SetCombatState(ECombatState::Attacking);

		if (AnimInst)
		{
			AnimInst->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);

			const int RandRate = FMath::RandRange(0, 2);

			UE_LOG(LogTemp, Warning, TEXT("RandRate: %d"), RandRate); // todo: remove
			if (RandRate == 0)
			{
				AnimInst->Montage_Play(MeleeAttackA_M);
			}
			else if (RandRate == 1)
			{
				AnimInst->Montage_Play(MeleeAttackB_M);
			}
			else
			{
				AnimInst->Montage_Play(MeleeAttackC_M);
			}
		}
	}
}

void UMeleeCombatComponent::Parry()
{
	// AnimInst 레퍼런스 준비
	UAnimInstance* AnimInst = nullptr;
	if (const ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		AnimInst = Char->GetMesh()->GetAnimInstance();
	}
	AnimInst->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);

	PauseMovement();
	// 유휴, 이동 이외의 상태일 경우 패링 불가능
	if (CombatState != ECombatState::IdleMoving) return;
	SetCombatState(ECombatState::Parrying);
	// 패링 몽타주 설정
	if (AMelee* Melee = Cast<AMelee>(GetOwner())) { Melee->PlayAnimMontage(MeleeParry_M, 1.1f); }
}

void UMeleeCombatComponent::Damage(int32 Damage, const FVector& DamageDirection, UCombatComponent* Performer)
{
	LastHitDirection = DamageDirection.GetSafeNormal();

	if (CombatState == ECombatState::Rolling)
	{
		return;
	}

	if (CombatState == ECombatState::Parrying)
	{
		PRINT_LOG(TEXT("패링"));
		SetCombatState(ECombatState::IdleMoving);
		ParrySuccess(Performer);
		return;
	}

	MeleeOnDamaged.Broadcast();
	if (UHealthComponent* HealthC = GetOwner()->FindComponentByClass<UHealthComponent>())
	{
		HealthC->UpdateHealth(-Damage);
	}
}

void UMeleeCombatComponent::ParrySuccess(UCombatComponent* Performer)
{
	auto Me = Cast<ACharacter>(GetOwner());
	auto You = Cast<ACharacter>(Performer->GetOwner());
	if (!Me || !You) return;

	UAnimInstance* AnimInst = Me->GetMesh()->GetAnimInstance();
	UAnimInstance* AnimInstOfYou = You->GetMesh()->GetAnimInstance();

	auto Me2YouRot = (You->GetActorLocation() - Me->GetActorLocation()).Rotation();
	auto You2MeRot = (Me->GetActorLocation() - You->GetActorLocation()).Rotation();

	MeleeOnParried.Broadcast();
	AnimInst->Montage_Pause();
	AnimInst->Montage_Play(MeleeParrySuccess_M);
	Me->GetCharacterMovement()->bOrientRotationToMovement = false;
	Me->bUseControllerRotationYaw = true;
	Me->FaceRotation(Me2YouRot);
	Me->bUseControllerRotationYaw = false;
	Me->GetCharacterMovement()->bOrientRotationToMovement = true;

	Performer->PauseMovement();
	Performer->SetCombatState(ECombatState::Staggered);
	Performer->OnStaggered.Broadcast();
	AnimInstOfYou->Montage_Play(StaggerMontage);
	You->GetCharacterMovement()->bOrientRotationToMovement = false;
	You->bUseControllerRotationYaw = true;
	You->FaceRotation(You2MeRot);
	You->bUseControllerRotationYaw = false;
	You->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void UMeleeCombatComponent::PauseMovement()
{
	if (auto MC = Cast<AMelee>(GetOwner()))
	{
		MC->SetIgnoreMove(true);
	}
}

void UMeleeCombatComponent::ResumeMovement()
{
	if (auto MC = Cast<AMelee>(GetOwner()))
	{
		MC->SetIgnoreMove(false);
	}
}

void UMeleeCombatComponent::SetCombatState(ECombatState NewState)
{
	CombatState = NewState;
	PRINT_LOG(TEXT("CombatState : %i"), CombatState)
}

void UMeleeCombatComponent::PerformAttackSweep()
{
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		FVector Base = Char->GetActorLocation();
		const FVector Forward = Char->GetActorForwardVector();
		FVector Start = Base + Forward * 60.0f;
		FVector End = Start + Forward * 120.0f;

		const FCollisionShape Capsule = FCollisionShape::MakeCapsule(50.0f, 50.0f);
		FCollisionQueryParams Params(NAME_None, false, Char);
		TArray<FHitResult> HitResults;
		bool bHit = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_GameTraceChannel3,
													Capsule, Params);

		for (auto& Hit : HitResults)
		{
			if (Hit.Actor.IsValid()) { PRINT_LOG(TEXT("Hit Actor Name : %s"), *Hit.Actor->GetName()); }
			if (const AActor* HitActor = Hit.GetActor())
			{
				if (UCombatComponent* HitCombatC = HitActor->FindComponentByClass<UCombatComponent>())
				{
					HitCombatC->Damage(AttackDamage, Forward, this);
				}
			}
		}
	}
}

void UMeleeCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted) return;

	if (Montage == DeathMontage) return;

	if (UAnimInstance* AnimInst = Cast<UAnimInstance>(Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance()))
	{
		SetCombatState(ECombatState::IdleMoving);
		ResumeMovement();
	}
}

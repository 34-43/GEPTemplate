#include "HYS/MinionCombatComponent.h"

#include "GEPTemplate.h"
#include "components/HealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYS/Minion.h"


class UHealthComponent;

UMinionCombatComponent::UMinionCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CombatState = ECombatState::IdleMoving;

	// 공격 몽타주 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MinionAttackA_M_Obj(
		TEXT("/Game/HYS/Minion/Montage/Minion_AttackA_Montage.Minion_AttackA_Montage"));
	if (MinionAttackA_M_Obj.Succeeded()) { MinionAttackA_M = MinionAttackA_M_Obj.Object; }

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MinionAttackB_M_Obj(
		TEXT("/Game/HYS/Minion/Montage/Minion_AttackB_Montage.Minion_AttackB_Montage"));
	if (MinionAttackB_M_Obj.Succeeded()) { MinionAttackB_M = MinionAttackB_M_Obj.Object; }

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MinionAttackC_M_Obj(
		TEXT("/Game/HYS/Minion/Montage/Minion_AttackC_Montage.Minion_AttackC_Montage"));
	if (MinionAttackC_M_Obj.Succeeded()) { MinionAttackC_M = MinionAttackC_M_Obj.Object; }

	// 패링 몽타주 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MinionParry_M_Obj(
		TEXT("/Game/HYS/Minion/Montage/Minion_Parry_Montage.Minion_Parry_Montage"));
	if (MinionParry_M_Obj.Succeeded()) { MinionParry_M = MinionParry_M_Obj.Object; }

	// 죽음 몽타주 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MinionDeathA_M_Obj(
		TEXT("/Game/HYS/Minion/Montage/Minion_DeathA_Montage.Minion_DeathA_Montage"));
	if (MinionDeathA_M_Obj.Succeeded()) { MinionDeathA_M = MinionDeathA_M_Obj.Object; }
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MinionDeathB_M_Obj(
	TEXT("/Game/HYS/Minion/Montage/Minion_DeathB_Montage.Minion_DeathB_Montage"));
	if (MinionDeathB_M_Obj.Succeeded()) { MinionDeathB_M = MinionDeathB_M_Obj.Object; }

}

void UMinionCombatComponent::BeginPlay()
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

void UMinionCombatComponent::Attack()
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
				AnimInst->Montage_Play(MinionAttackA_M);
			}
			else if (RandRate == 1)
			{
				AnimInst->Montage_Play(MinionAttackB_M);
			}
			else
			{
				AnimInst->Montage_Play(MinionAttackC_M);
			}
		}
	}
}

void UMinionCombatComponent::Parry()
{
	// todo: override parry
	PauseMovement();
	// 유휴, 이동 이외의 상태일 경우 패링 불가능
	if (CombatState != ECombatState::IdleMoving) return;
	SetCombatState(ECombatState::Parrying);
	// 패링 몽타주 설정
	if (AMinion* Minion = Cast<AMinion>(GetOwner())) { Minion->PlayAnimMontage(MinionParry_M, 1.1f); }
}

void UMinionCombatComponent::Damage(int32 Damage, const FVector& DamageDirection, UCombatComponent* Performer)
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

	MinionOnDamaged.Broadcast();
	if (UHealthComponent* HealthC = GetOwner()->FindComponentByClass<UHealthComponent>())
	{
		HealthC->UpdateHealth(-Damage);
	}
}

void UMinionCombatComponent::ParrySuccess(UCombatComponent* Performer)
{
	auto Me = Cast<ACharacter>(GetOwner());
	auto You = Cast<ACharacter>(Performer->GetOwner());
	if (!Me || !You) return;

	UAnimInstance* AnimInst = Me->GetMesh()->GetAnimInstance();
	UAnimInstance* AnimInstOfYou = You->GetMesh()->GetAnimInstance();

	auto Me2YouRot = (You->GetActorLocation() - Me->GetActorLocation()).Rotation();
	auto You2MeRot = (Me->GetActorLocation() - You->GetActorLocation()).Rotation();

	MinionOnParried.Broadcast();
	AnimInst->Montage_Pause();
	AnimInst->Montage_Play(ParryMontage);
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

void UMinionCombatComponent::PauseMovement()
{
	if (auto MC = Cast<AMinion>(GetOwner()))
	{
		MC->SetIgnoreMove(true);
	}
}

void UMinionCombatComponent::ResumeMovement()
{
	if (auto MC = Cast<AMinion>(GetOwner()))
	{
		MC->SetIgnoreMove(false);
	}
}

void UMinionCombatComponent::SetCombatState(ECombatState NewState)
{
	CombatState = NewState;
	PRINT_LOG(TEXT("CombatState : %i"), CombatState)
}

void UMinionCombatComponent::PerformAttackSweep()
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

void UMinionCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted) return;

	if (Montage == DeathMontage) return;

	if (UAnimInstance* AnimInst = Cast<UAnimInstance>(Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance()))
	{
		SetCombatState(ECombatState::IdleMoving);
		ResumeMovement();
	}
}

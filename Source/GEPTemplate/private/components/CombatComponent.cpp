#include "components/CombatComponent.h"

#include "GEPTemplate.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "allies/MainCharacter.h"
#include "components/HealthComponent.h"
#include "enemies/BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CombatState = ECombatState::IdleMoving;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageObject(
		TEXT("/Game/Features/Mannequin/Animations/Montage/AttackMontage.AttackMontage"));
	if (AttackMontageObject.Succeeded()) { AttackMontage = AttackMontageObject.Object; }
	AttackMontageSections = {FName("1"), FName("2"), FName("3"), FName("4")};
	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontageObject(
		TEXT("/Game/Features/Mannequin/Animations/Montage/RollMontage.RollMontage"));
	if (RollMontageObject.Succeeded()) { RollMontage = RollMontageObject.Object; }
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ParryMontageObject(
		TEXT("/Game/Features/Mannequin/Animations/Montage/ParryMontage.ParryMontage"));
	if (ParryMontageObject.Succeeded()) { ParryMontage = ParryMontageObject.Object; }
	ParryMontageSections = {FName("1"), FName("2")};
	static ConstructorHelpers::FObjectFinder<UAnimMontage> StaggerMontageObject(
		TEXT("/Game/Features/Mannequin/Animations/Montage/StaggerMontage.StaggerMontage"));
	if (StaggerMontageObject.Succeeded()) { StaggerMontage = StaggerMontageObject.Object; }
}


void UCombatComponent::BeginPlay()
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

void UCombatComponent::SetCombatState(const ECombatState NewState)
{
	CombatState = NewState;
	PRINT_LOG(TEXT("CombatState : %i"), CombatState)
}

void UCombatComponent::Attack()
{
	// AnimInst 레퍼런스 준비
	UAnimInstance* AnimInst = nullptr;
	if (const ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		AnimInst = Char->GetMesh()->GetAnimInstance();
	}

	// 기본 상태에서 첫 동작 재생
	if (CombatState == ECombatState::IdleMoving)
	{
		SetCombatState(ECombatState::Attacking);

		if (AnimInst)
		{
			AnimInst->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
			AnimInst->Montage_JumpToSection(AttackMontageSections[0], AttackMontage);
			AnimInst->Montage_Play(AttackMontage);
		}
	}
	// 조건이 맞으면 콤보 큐를 설정함
	else if (CombatState == ECombatState::Attacking && AnimInst)
	{
		if (AnimInst->Montage_IsPlaying(AttackMontage))
		{
			FName PlayingSection = AnimInst->Montage_GetCurrentSection(AttackMontage);
			int32 PlayingIndex = AttackMontageSections.Find(PlayingSection);

			if (PlayingIndex != INDEX_NONE && PlayingIndex < AttackMontageSections.Num() - 1)
			{
				float Start, End;
				AttackMontage->GetSectionStartAndEndTime(PlayingIndex, Start, End);
				float Pos = AnimInst->Montage_GetPosition(AttackMontage);

				PRINT_LOG(TEXT("Start: %f / End: %f / Pos : %f"), Start, End, Pos);

				if (abs(End - Pos) < 0.5f)
				{
					AnimInst->Montage_SetNextSection(PlayingSection, AttackMontageSections[PlayingIndex + 1],
					                                 AttackMontage);
				}
			}
		}
	}
}

void UCombatComponent::Parry()
{
	PauseMovement();
	// 유휴, 이동 이외의 상태일 경우 패링 불가능
	if (CombatState != ECombatState::IdleMoving) return;
	SetCombatState(ECombatState::Parrying);
	// 패링 몽타주 설정
	if (ACharacter* Char = Cast<ACharacter>(GetOwner())) { Char->PlayAnimMontage(ParryMontage, 0.8f); }
}


void UCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted) return;

	if (auto MainChar = Cast<AMainCharacter>(GetOwner()))
	{
		MainChar->SetOverrideMovement(false);
	}

	if (UAnimInstance* AnimInst = Cast<UAnimInstance>(Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance()))
	{
		SetCombatState(ECombatState::IdleMoving);
		ResumeMovement();
	}
}

void UCombatComponent::PerformAttackSweep()
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
		// 충돌 영역 시각화 시작 =================
#if ENABLE_DRAW_DEBUG
		const FColor C = bHit ? FColor::Red : FColor::Green;
		// DrawDebugCapsule(GetWorld(), Start, 50.0f, 50.0f, FQuat::Identity, C.WithAlpha(0.3f), false, 1.0f);
		// DrawDebugCapsule(GetWorld(), End, 50.0f, 50.0f, FQuat::Identity, C.WithAlpha(0.3f), false, 1.0f);
		// DrawDebugLine(GetWorld(), Start, End, C.WithAlpha(0.3f), false, 1.0f, 0, 2.0f);
#endif
		// 충돌 영역 시각화 종료 =================

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

/**
 * 
 * @param Damage
 * @param DamageDirection 
 * @return 데미지가 무시되는 경우, false를 반환
 */
void UCombatComponent::Damage(int32 Damage, const FVector& DamageDirection, UCombatComponent* Performer)
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

	OnDamaged.Broadcast();
	if (UHealthComponent* HealthC = GetOwner()->FindComponentByClass<UHealthComponent>())
	{
		HealthC->UpdateHealth(-Damage);
	}
}

void UCombatComponent::ParrySuccess(UCombatComponent* Performer)
{
	auto Me = Cast<ACharacter>(GetOwner());
	auto You = Cast<ACharacter>(Performer->GetOwner());
	if (!Me || !You) return;

	UAnimInstance* AnimInst = Me->GetMesh()->GetAnimInstance();
	UAnimInstance* AnimInstOfYou = You->GetMesh()->GetAnimInstance();

	auto Me2YouRot = (You->GetActorLocation() - Me->GetActorLocation()).Rotation();
	auto You2MeRot = (Me->GetActorLocation() - You->GetActorLocation()).Rotation();

	OnParried.Broadcast();
	AnimInst->Montage_Pause();
	AnimInst->Montage_Play(ParryMontage);
	AnimInst->Montage_JumpToSection(ParryMontageSections[1]);
	Me->GetCharacterMovement()->bOrientRotationToMovement = false;
	Me->FaceRotation(Me2YouRot);
	Me->GetCharacterMovement()->bOrientRotationToMovement = true;

	Performer->PauseMovement();
	Performer->SetCombatState(ECombatState::Staggered);
	Performer->OnStaggered.Broadcast();
	AnimInstOfYou->Montage_Play(StaggerMontage);
	You->GetCharacterMovement()->bOrientRotationToMovement = false;
	You->FaceRotation(You2MeRot);
	You->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void UCombatComponent::PauseMovement()
{
	if (auto Char = Cast<ACharacter>(GetOwner()))
	{
		Char->GetCharacterMovement()->DisableMovement();
	}
}

void UCombatComponent::ResumeMovement()
{
	if (auto Char = Cast<ACharacter>(GetOwner()))
	{
		Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

#include "components/CombatComponent.h"

#include "GEPTemplate.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CombatState = ECombatState::IdleMoving;
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

void UCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CombatState == ECombatState::Rolling)
	{
		if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
		{
			const FVector Delta = Char->GetActorForwardVector() * RollSpeed * DeltaTime;
			Char->AddMovementInput(Delta);
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
					AnimInst->Montage_SetNextSection(PlayingSection, AttackMontageSections[PlayingIndex + 1], AttackMontage);
				}
			}
		}
	}

}

void UCombatComponent::Roll()
{
	// 유휴, 이동 이외의 상태일 경우 회피 불가능
	if (CombatState != ECombatState::IdleMoving) return;
	SetCombatState(ECombatState::Rolling);
	// 구르기 몽타주 설정
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		Char->PlayAnimMontage(RollMontage);
	}
	// 무적시간 종료 타이머
	GetWorld()->GetTimerManager().SetTimer(StateTimerHandle, [this]()
	{
		SetCombatState(ECombatState::BufferTime);
	}, RollAvoidTime, false);
}

void UCombatComponent::Parry()
{
	// 유휴, 이동 이외의 상태일 경우 패링 불가능
	if (CombatState != ECombatState::IdleMoving) return;
	SetCombatState(ECombatState::Parrying);
	// 패링 몽타주 설정
	if (ACharacter* Char = Cast<ACharacter>(GetOwner())) { Char->PlayAnimMontage(ParryMontage); }
}


void UCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (UAnimInstance* AnimInst = Cast<UAnimInstance>(Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance()))
	{
		AnimInst->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	if (Montage == AttackMontage)
	{
		PerformAttackTrace();
	}
	SetCombatState(ECombatState::IdleMoving);
}

void UCombatComponent::PerformAttackTrace() const
{
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		FVector StartLocation = Char->GetActorLocation() + FVector(0, 0, 50);
		FVector EndLocation = StartLocation + Char->GetActorForwardVector() * AttackRange;
		FHitResult Hit;
		FCollisionQueryParams Params(NAME_None, false, Char);

		// 같은 전투 컴포넌트 사용자 간 데미지 계산
		if (Char->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Pawn, Params))
		{
			if (UCombatComponent* Other = Hit.GetActor()->FindComponentByClass<UCombatComponent>())
			{
				Other->Damage(AttackDamage, Char->GetActorForwardVector());
			}
		}
	}
}

void UCombatComponent::Damage(float Damage, const FVector& DamageDirection)
{
	// 패링 성공 시
	if (CombatState == ECombatState::Parrying)
	{
		PRINT_LOG(TEXT("퍼펙트 패링!!"));
		SetCombatState(ECombatState::IdleMoving);
	}

	if (CombatState == ECombatState::Rolling)
	{
		PRINT_LOG(TEXT("퍼펙트 위빙!!"));
		SetCombatState(ECombatState::IdleMoving);
	}

	SetCombatState(ECombatState::Stunned);
	GetWorld()->GetTimerManager().SetTimer(StateTimerHandle, this, &UCombatComponent::EndStun, StunOnDamageTime, false);
}

void UCombatComponent::EndStun() { SetCombatState(ECombatState::IdleMoving); }

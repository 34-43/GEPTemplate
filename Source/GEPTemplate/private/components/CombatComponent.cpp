#include "components/CombatComponent.h"

#include "GEPTemplate.h"
#include "GameFramework/Character.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CombatState = ECombatState::Idle;

	// todo: 뭔가 오류남
	// static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageAsset(
	// 	TEXT("/Game/Features/Mannequin/Animations/AttackMontage.AttackMontage"));
	// if (AttackMontageAsset.Succeeded()) { AttackMontage = AttackMontageAsset.Object; }
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

void UCombatComponent::SetCombatState(const ECombatState NewState) { CombatState = NewState; }

void UCombatComponent::Attack()
{
	// 유휴, 이동 이외의 상태일 경우 공격 불가능
	if (CombatState > ECombatState::Moving) return;
	SetCombatState(ECombatState::Attacking);
	// 공격 몽타주 설정
	if (ACharacter* Char = Cast<ACharacter>(GetOwner())) { Char->PlayAnimMontage(AttackMontage); }
}

void UCombatComponent::Roll()
{
	// 유휴, 이동 이외의 상태일 경우 회피 불가능
	if (CombatState > ECombatState::Moving) return;
	SetCombatState(ECombatState::Rolling);
	// 구르기 몽타주 설정
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		Char->PlayAnimMontage(RollMontage);
		// Char->SetActorEnableCollision(false); //구르기와 동시에 무적 시작
	}
	// 무적시간 종료 타이머 설정
	// GetWorld()->GetTimerManager().SetTimer(StateTimerHandle, [this]()
	// {
	// 	if (ACharacter* Char = Cast<ACharacter>(GetOwner())) { Char->SetActorEnableCollision(true); }
	// }, RollAvoidTime, false);
}

void UCombatComponent::Parry()
{
	// 유휴, 이동 이외의 상태일 경우 패링 불가능
	if (CombatState > ECombatState::Moving) return;
	SetCombatState(ECombatState::Parrying);
	// 패링 몽타주 설정
	if (ACharacter* Char = Cast<ACharacter>(GetOwner())) { Char->PlayAnimMontage(ParryMontage); }
}


void UCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage) { PerformAttackTrace(); } // 왜 끝날 때 하는 지는 모름.
	SetCombatState(ECombatState::Idle);
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
		PRINT_LOG(TEXT("패링 성공!!"));
		SetCombatState(ECombatState::Idle);
	}

	SetCombatState(ECombatState::Stunned);
	GetWorld()->GetTimerManager().SetTimer(StateTimerHandle, this, &UCombatComponent::EndStun, StunOnDamageTime, false);
}

void UCombatComponent::EndStun() { SetCombatState(ECombatState::Idle); }

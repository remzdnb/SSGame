#include "Pawn/SS_PawnAnimInstance.h"
#include "Pawn/SS_Pawn.h"
//
#include "Kismet/KismetMathLibrary.h"

void USS_PawnAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = Cast<ASS_Pawn>(TryGetPawnOwner());
	if (OwningPawn)
	{
		PlaySlotAnimationAsDynamicMontage(
			OwningPawn->PawnData.IdleAnimSequence,
			"DefaultSlot",
			0.0f,
			0.0f,
			1.0f,
			1
		);
	}
	
	LastAnimation = ESS_PawnState::Idle;
}

void USS_PawnAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
}

void USS_PawnAnimInstance::PlayIdleAnimation()
{
	if (OwningPawn->PawnData.IdleAnimSequence == nullptr)
		return;
	
	if (LastAnimation == ESS_PawnState::Idle)
		return;
	
	PlaySlotAnimationAsDynamicMontage(
		OwningPawn->PawnData.IdleAnimSequence,
		"DefaultSlot",
		0.1f,
		0.1f,
		1.0f,
		99999999
	);

	LastAnimation = ESS_PawnState::Idle;
}

void USS_PawnAnimInstance::PlayMoveAnimation()
{
	if (OwningPawn->PawnData.MoveAnimSequence == nullptr)
		return;
		
	if (LastAnimation == ESS_PawnState::Move)
		return;

	PlaySlotAnimationAsDynamicMontage(
		OwningPawn->PawnData.MoveAnimSequence,
		"DefaultSlot",
		0.1f,
		0.1f,
		1.0f,
		99999999
	);
	
	LastAnimation = ESS_PawnState::Move;
}

void USS_PawnAnimInstance::PlayDeathAnimation()
{
	if (OwningPawn->PawnData.DeathAnimSequence == nullptr)
		return;

	PlaySlotAnimationAsDynamicMontage(
		OwningPawn->PawnData.DeathAnimSequence,
		"DefaultSlot",
		0.1f,
		0.1f,
		1.0f,
		1
	);
	
	LastAnimation = ESS_PawnState::Dead;
}

void USS_PawnAnimInstance::PlayMeleeAttackStartAnimation()
{
	if (OwningPawn->PawnData.MeleeAttackStartAnimSequence == nullptr)
		return;
	
	PlaySlotAnimationAsDynamicMontage(
		OwningPawn->PawnData.MeleeAttackStartAnimSequence,
		"DefaultSlot",
		0.0f,
		0.0f,
		OwningPawn->PawnData.MeleeAttackStartAnimSequence->SequenceLength / OwningPawn->PawnData.MeleeAttackStartDelay,
		1
	);
	
	LastAnimation = ESS_PawnState::MeleeAttackStart;
}

void USS_PawnAnimInstance::PlayMeleeAttackStopAnimation()
{
	if (OwningPawn->PawnData.MeleeAttackStopAnimSequence == nullptr)
		return;
	
	PlaySlotAnimationAsDynamicMontage(
		OwningPawn->PawnData.MeleeAttackStopAnimSequence,
		"DefaultSlot",
		0.0f,
		0.0f,
		OwningPawn->PawnData.MeleeAttackStopAnimSequence->SequenceLength / OwningPawn->PawnData.MeleeAttackStopDelay,
		1
	);

	LastAnimation = ESS_PawnState::MeleeAttackStop;
}

void USS_PawnAnimInstance::PlayRangedAttackStartAnimation()
{
	if (OwningPawn->PawnData.RangedAttackStartAnimSequence == nullptr)
		return;
	
	PlaySlotAnimationAsDynamicMontage(
		OwningPawn->PawnData.RangedAttackStartAnimSequence,
		"DefaultSlot",
		0.0f,
		0.0f,
		OwningPawn->PawnData.RangedAttackStartAnimSequence->SequenceLength / OwningPawn->PawnData.MeleeAttackStartDelay,
		1
	);

	LastAnimation = ESS_PawnState::RangedAttackStart;
}

void USS_PawnAnimInstance::PlayRangedAttackStopAnimation()
{
	if (OwningPawn->PawnData.RangedAttackStopAnimSequence == nullptr)
		return;

	PlaySlotAnimationAsDynamicMontage(
		OwningPawn->PawnData.RangedAttackStopAnimSequence,
		"DefaultSlot",
		0.0f,
		0.0f,
		OwningPawn->PawnData.RangedAttackStopAnimSequence->SequenceLength / OwningPawn->PawnData.MeleeAttackStartDelay,
		1
	);

	LastAnimation = ESS_PawnState::RangedAttackStop;
}

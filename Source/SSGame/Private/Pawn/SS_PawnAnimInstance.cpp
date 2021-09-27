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
		Montage_Play(OwningPawn->PawnData.AnimMontage);
		Montage_JumpToSection("Idle", OwningPawn->PawnData.AnimMontage);
	}

	LastAnimation = ESS_PawnAnimation::Idle;
}

void USS_PawnAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
}

void USS_PawnAnimInstance::PlayIdleAnimation()
{
	if (LastAnimation == ESS_PawnAnimation::Move)
	{
		Montage_JumpToSection("MoveStop", OwningPawn->PawnData.AnimMontage);
	}
	else if (LastAnimation != ESS_PawnAnimation::Idle)
	{
		
	}
	else
	{
		Montage_JumpToSection("Idle", OwningPawn->PawnData.AnimMontage);
	}
	
	LastAnimation = ESS_PawnAnimation::Idle;
}

void USS_PawnAnimInstance::PlayMoveStartAnimation()
{
	if (LastAnimation != ESS_PawnAnimation::Move)
		Montage_JumpToSection("MoveStart", OwningPawn->PawnData.AnimMontage);

	LastAnimation = ESS_PawnAnimation::Move;
}

void USS_PawnAnimInstance::PlayMoveEndAnimation()
{
	LastAnimation = ESS_PawnAnimation::Idle;
	Montage_JumpToSection("MoveStop", OwningPawn->PawnData.AnimMontage);
}

void USS_PawnAnimInstance::PlayAttackStartAnimation()
{
	LastAnimation = ESS_PawnAnimation::Attack;
	Montage_JumpToSection("AttackStart", OwningPawn->PawnData.AnimMontage);
}

void USS_PawnAnimInstance::PlayAttackEndAnimation()
{
	LastAnimation = ESS_PawnAnimation::Attack;
	Montage_JumpToSection("AttackEnd", OwningPawn->PawnData.AnimMontage);
}

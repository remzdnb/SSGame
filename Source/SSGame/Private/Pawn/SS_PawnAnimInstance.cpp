#include "Pawn/SS_PawnAnimInstance.h"
#include "Pawn/SS_Pawn.h"
//
#include "Kismet/KismetMathLibrary.h"

void USS_PawnAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = Cast<ASS_Pawn>(TryGetPawnOwner());
	CurrentState = ESS_PawnState::Idling;
}

void USS_PawnAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (OwningPawn == nullptr)
		return;
	
	if (CurrentState != OwningPawn->State)
	{
		if (OwningPawn->State == ESS_PawnState::Moving)
			Montage_Play(OwningPawn->PawnData.AnimMontage);
		else
			Montage_Stop(0.0f);

		CurrentState = OwningPawn->State;
	}
}

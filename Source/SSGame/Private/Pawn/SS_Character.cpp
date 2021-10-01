#include "Pawn/SS_Character.h"
#include "Pawn/SS_PawnAnimInstance.h"
#include "World/SS_Tile.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
//
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "World/SS_Grid.h"

ASS_Character::ASS_Character()
{
	MoveArrowCT = CreateDefaultSubobject<UArrowComponent>(FName("MoveArrowCT"));
	MoveArrowCT->SetupAttachment(RootComponent);
}

void ASS_Character::Init_Multicast(const FName& PawnDataRowName, const FSS_TileGroupData& InitialTileGroup, ESS_Team NewTeam,
                         bool bIsDemoPawn, bool bIsDemoPawnValidLocation)
{
	Super::Init_Multicast_Implementation(PawnDataRowName, InitialTileGroup, NewTeam, bIsDemoPawn, bIsDemoPawnValidLocation);

	MoveArrowCT->SetRelativeLocation(FVector(
		(PawnData.Size - 1) * (TILESIZE / 2),
		(PawnData.Size - 1) * (TILESIZE / 2),
		0.0f
	));
	
	MoveCurve = GInstance->GameSettings->LinearCurveFloat;
	if (MoveCurve)
	{
		FOnTimelineFloat MoveTimelineProgressCallback;
		FOnTimelineEventStatic MoveTimelineFinishedCallback;
		MoveTimelineProgressCallback.BindUFunction(this, FName("MoveTimelineProgress"));
		MoveTimelineFinishedCallback.BindUFunction(this, FName("MoveTimelineEnd"));
		MoveTimeline.AddInterpFloat(MoveCurve, MoveTimelineProgressCallback);
		MoveTimeline.SetTimelineFinishedFunc(MoveTimelineFinishedCallback);
		MoveTimeline.SetLooping(false);
		MoveTimeline.SetPlayRate(PawnData.MoveSpeed);
	}
}

void ASS_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTimeline.TickTimeline(DeltaTime);
}

#pragma region +++++ Movement ...

void ASS_Character::StartNewMove(ESS_PawnMoveType NewMoveType)
{
	if (NewMoveType == ESS_PawnMoveType::Idle)
	{
		State = ESS_PawnState::Idle;
		AInstance->PlayIdleAnimation();
	}
	else
	{
		if (MoveData.Length > 0)
		{
			if (Grid->RequestCharacterMovement(this, MoveData.Direction))
			{
				State = ESS_PawnState::Move;
				AInstance->PlayMoveAnimation();
			}
		}
		else
		{
			State = ESS_PawnState::Idle;
			AInstance->PlayIdleAnimation();
		}
	}
}

void ASS_Character::StartMoveToTileGroup()
{
	MoveStartLocation = GetActorLocation();
	MoveDirection = UKismetMathLibrary::FindLookAtRotation(
		MoveStartLocation,
		TileGroup.OriginTile->CenterLocation
	).Vector();
	MoveDistance = FVector::Dist(MoveStartLocation, TileGroup.OriginTile->CenterLocation);

	MoveTimeline.PlayFromStart();
}

void ASS_Character::UpdateMoveArrow(bool bNewIsVisible, ASS_Tile* TargetTile)
{
	MoveArrowCT->SetHiddenInGame(!bNewIsVisible);

	if (bNewIsVisible && TargetTile)
	{
		MoveArrowCT->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
				MoveArrowCT->GetComponentLocation(),
				TargetTile->CenterLocation)
		);
		MoveArrowCT->SetHiddenInGame(true);
		MoveArrowCT->SetHiddenInGame(false);
		MoveArrowCT->ArrowLength =
			FVector::Dist(RotationAxisCT->GetComponentLocation(), TargetTile->CenterLocation) / MoveArrowCT->ArrowSize;
	}
}

void ASS_Character::MoveTimelineProgress(float Value)
{
	const FVector NewLocation = MoveStartLocation + MoveDirection * MoveDistance * Value;
	SetActorLocation(NewLocation);
}

void ASS_Character::MoveTimelineEnd()
{
	MoveData.Length--;
	OnPawnActionCompletedEvent.Broadcast();
}

#pragma endregion
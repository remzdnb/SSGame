#include "Pawn/SS_PawnAIController.h"
#include "Pawn/SS_Pawn.h"
#include "World/SS_Grid.h"
//
#include "EngineUtils.h"

ASS_PawnAIController::ASS_PawnAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bWantsPlayerState = false;

	//

	bWantsToMove = true;
}

void ASS_PawnAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (TActorIterator<ASS_Grid> FoundGrid(GetWorld()); FoundGrid; ++FoundGrid)
	{
		Grid = *FoundGrid;
	}
}

void ASS_PawnAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ASS_PawnAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ControlledPawn == nullptr || Grid == nullptr)
		return;

	if (ControlledPawn->State == ESS_PawnState::Idle)
	{
		StartNextAction();
	}
}

void ASS_PawnAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledPawn = Cast<ASS_Pawn>(InPawn);
	if (ControlledPawn)
	{
		if (ControlledPawn->State == ESS_PawnState::Demo)
			return;

		ControlledPawn->OnPawnActionCompletedEvent.AddUniqueDynamic(this, &ASS_PawnAIController::StartNextAction);
		StartNextAction();
	}
}

void ASS_PawnAIController::StartNextAction()
{
	if (ControlledPawn->StartNewAttack() == false)
	{
		if (bWantsToMove && Grid->RequestPawnMovement(ControlledPawn))
		{
			ControlledPawn->StartNewMove(ESS_PawnMoveType::Move);
		}
		else
		{
			ControlledPawn->StartNewMove(ESS_PawnMoveType::Idle);
		}
	}
}

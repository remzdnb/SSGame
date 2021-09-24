#include "Pawn/SS_PawnAIController.h"
#include "Pawn/SS_Pawn.h"
#include "World/SS_Grid.h"
//
#include "EngineUtils.h"

ASS_PawnAIController::ASS_PawnAIController()
{
	PrimaryActorTick.bCanEverTick = true;
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
	
	if (ControlledPawn->State == ESS_PawnState::Idling)
	{
		Grid->RequestPawnMovement(ControlledPawn);
	}
}

void ASS_PawnAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledPawn = Cast<ASS_Pawn>(InPawn);
}


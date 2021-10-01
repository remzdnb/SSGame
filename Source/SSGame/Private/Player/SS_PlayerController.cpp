#include "Player/SS_PlayerController.h"
#include "Player/SS_PlayerState.h"
#include "Player/SS_CameraPawn.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
#include "Pawn/SS_Character.h"
#include "Pawn/SS_PawnAIController.h"
#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "UI/SS_MainHUDWidget.h"
#include "SS_UtilityLibrary.h"
// Engine
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ASS_PlayerController::ASS_PlayerController()
{
	PCMode = ESS_PlayerControllerMode::Spawn;
}

void ASS_PlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld()->IsGameWorld() == false)
		return;

	GInstance = Cast<USS_GameInstance>(GetGameInstance());

	for (TActorIterator<ASS_Grid> FoundGrid(GetWorld()); FoundGrid; ++FoundGrid)
	{
		Grid = *FoundGrid;
	}

	SelectedPawnDataRowName = GInstance->PawnDT->GetRowNames()[0];
}

void ASS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
		OnRep_PlayerState();

	if (IsLocalController() == false)
		return;
	
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this);
	FInputModeGameAndUI NewInputMode;
	NewInputMode.SetHideCursorDuringCapture(false);
	SetInputMode(NewInputMode);
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
	HitResultTraceDistance = 1000000.0f;
	
	// UI

	HUDWidget = CreateWidget<USS_MainHUDWidget>(
		GetWorld(),
		GInstance->GameSettings->MainHUD_WBP
	);
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}

	// Debug

	if (GInstance->GameSettings->bDebugProjectile)
	{
		ConsoleCommand("show Splines");	
	}
}

void ASS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocalController() == false || PState == nullptr)
		return;
	
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
	ASS_Tile* NewHoveredTile = Cast<ASS_Tile>(HitResult.Actor);
	if (NewHoveredTile != HoveredTile)
	{
		HoveredTile = NewHoveredTile;
		UpdateHoveredTile();
	}
}

void ASS_PlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PState = Cast<ASS_PlayerState>(PlayerState);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		for (TActorIterator<ASS_CameraPawn> FoundCameraPawn(GetWorld()); FoundCameraPawn; ++FoundCameraPawn)
		{
			if (FoundCameraPawn->Team == PState->Team)
			{
				CameraPawn = *FoundCameraPawn;
				Possess(CameraPawn);
			}
		}
	}
}

void ASS_PlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	CameraPawn = Cast<ASS_CameraPawn>(GetPawn());
}

void ASS_PlayerController::SelectCharacterToSpawn(const FName& NewCharacterRowName)
{
	SelectedPawnDataRowName = NewCharacterRowName;
	SelectedPawnData = *GInstance->GetPawnDataFromRow(SelectedPawnDataRowName);
	OnNewSelectedPawnEvent.Broadcast();
}

void ASS_PlayerController::UpdateHoveredTile()
{
	if (HoveredTile == nullptr)
		return;

	//if (HoveredTile->TileData.Team != PState->Team)
		//return;

	Grid->DisableAllHighlight();

	if (DragStartTile)
	{
		SelectedTiles = Grid->GetTilesFromSquare(DragStartTile, HoveredTile);
	}
	else
	{
		SelectedTiles.Empty();
		SelectedTiles.Add(HoveredTile);
	}

	for (const auto& SelectedTile : SelectedTiles)
	{
		Grid->HighlightLine(SelectedTile->TileData.YPosition, true);
	}
		
	for (const auto& SelectedTile : SelectedTiles)
	{
		SelectedTile->HighlightColumnParticles(true, true);
		SelectedTile->HighlightLineParticles(true, true);
	}

	if (PCMode == ESS_PlayerControllerMode::Spawn)
	{
		UpdateDemoPawns();
	}
	else
	{
		if (SelectedPawns.Num() != 0)
		{
			UpdateDemoMovement();
		}
	}
}

void ASS_PlayerController::UpdateDemoPawns()
{
	for (const auto& DemoPawn : DemoPawns)
	{
		if (DemoPawn)
		{
			DemoPawn->Destroy();
		}
	}
	DemoPawns.Empty();

	TArray<FSS_TileGroupData> ValidTileGroups;
	Grid->GetValidTileGroupsFromSquare(ValidTileGroups, SelectedTiles, SelectedPawnData.Size);
	for (const auto& TileGroup : ValidTileGroups)
	{
		DemoPawns.Add(Grid->SpawnPawn(TileGroup, SelectedPawnDataRowName, true));
	}
}

void ASS_PlayerController::UpdateSelectedPawns()
{
	UnselectAllPawns();
	
	for (const auto& SelectedTile : SelectedTiles)
	{
		ASS_Pawn* SelectedPawn = SelectedTile->GetRegisteredPawn().Get();
		if (SelectedPawn)
		{
			SelectedPawn->ToggleHighlight(true);
			SelectedPawns.Add(SelectedPawn);
		}
	}
}

void ASS_PlayerController::UnselectAllPawns()
{
	for (const auto& SelectedPawn : SelectedPawns)
	{
		if (SelectedPawn.IsValid())
		{
			SelectedPawn->ToggleHighlight(false);
		}
	}
	SelectedPawns.Empty();
}

void ASS_PlayerController::UpdateDemoMovement()
{
	TArray<ASS_Tile*> PawnOriginTiles;
	for (const auto& SelectedPawn : SelectedPawns)
	{
		PawnOriginTiles.Add(SelectedPawn->TileGroup.OriginTile);
	}
	ASS_Tile* ClosestPawnOriginTile = Grid->GetClosestTile(HoveredTile, PawnOriginTiles);
	
	Grid->GetMoveData(SavedMoveData, ClosestPawnOriginTile, HoveredTile);
	
	for (const auto& SelectedPawn : SelectedPawns)
	{
		ASS_Character* CharacterToMove = Cast<ASS_Character>(SelectedPawn);
		if (CharacterToMove)
		{
			CharacterToMove->UpdateMoveArrow(true, Grid->GetTileFromMoveData(CharacterToMove->TileGroup.OriginTile, SavedMoveData));
		}
	}
}

void ASS_PlayerController::SpawnPawn_Implementation()
{
}

#pragma region +++++ Input ...

void ASS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAxis("LookUpAxis", this, &ASS_PlayerController::LookUpAxis).bConsumeInput = false;
	InputComponent->BindAxis("LookRightAxis", this, &ASS_PlayerController::LookRightAxis).bConsumeInput = false;
	InputComponent->BindAxis("ZoomAxis", this, &ASS_PlayerController::ZoomAxis);
	InputComponent->BindAxis("MoveForwardAxis", this, &ASS_PlayerController::MoveForwardAxis);
	InputComponent->BindAxis("MoveRightAxis", this, &ASS_PlayerController::MoveRightAxis);
	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ASS_PlayerController::OnLeftMouseButtonPressed);
	InputComponent->BindAction("LeftMouseButton", IE_Released, this, &ASS_PlayerController::OnLeftMouseButtonReleased);
	InputComponent->BindAction("RightMouseButton", IE_Pressed, this, &ASS_PlayerController::RightMouseButtonPressed);
	InputComponent->BindAction("RightMouseButton", IE_Released, this, &ASS_PlayerController::RightMouseButtonReleased);
	InputComponent->BindAction("MiddleMouseButton", IE_Pressed, this, &ASS_PlayerController::MiddleMouseButtonPressed);
	InputComponent->BindAction("MiddleMouseButton", IE_Released, this, &ASS_PlayerController::MiddleMouseButtonReleased);
	InputComponent->BindAction("Tab", IE_Pressed, this, &ASS_PlayerController::TabKeyPressed);
	InputComponent->BindAction("Shift", IE_Pressed, this, &ASS_PlayerController::ShiftKeyPressed);
	InputComponent->BindAction("Shift", IE_Released, this, &ASS_PlayerController::ShiftKeyReleased);
	InputComponent->BindAction("SpaceBar", IE_Pressed, this, &ASS_PlayerController::SpaceBarKeyPressed);
}

void ASS_PlayerController::LookUpAxis(float AxisValue)
{
	if (CameraPawn)
	{
		CameraPawn->AddPitchInput(AxisValue);	
	}
}

void ASS_PlayerController::LookRightAxis(float AxisValue)
{
	if (CameraPawn)
	{
		CameraPawn->AddYawInput(AxisValue);
	}
}

void ASS_PlayerController::ZoomAxis(float AxisValue)
{
	if (CameraPawn)
	{
		if (AxisValue > 0)
		{
			CameraPawn->AddZoomInput(true);	
		}

		if (AxisValue < 0)
		{
			CameraPawn->AddZoomInput(false);
		}
	}
}

void ASS_PlayerController::MoveForwardAxis(float AxisValue)
{
	if (CameraPawn)
	{
		CameraPawn->MoveForward(AxisValue);
	}
}

void ASS_PlayerController::MoveRightAxis(float AxisValue)
{
	if (CameraPawn)
	{
		CameraPawn->MoveRight(AxisValue);
	}
}

void ASS_PlayerController::OnLeftMouseButtonPressed()
{
	DragStartTile = HoveredTile;
}

void ASS_PlayerController::OnLeftMouseButtonReleased()
{
	if (PCMode == ESS_PlayerControllerMode::Spawn)
	{
		for (const auto& DemoPawn : DemoPawns)
		{
			Grid->SpawnPawnServer(DemoPawn->TileGroup.OriginTile, SelectedPawnDataRowName, false);
			//GamePawns.Add(Grid->SpawnPawn(DemoPawn->TileGroup, SelectedPawnDataRowName));
		}

		for (const auto& DemoPawn : DemoPawns)
		{
			DemoPawn->Destroy();
		}

		DemoPawns.Empty();
	}
	else
	{
		UpdateSelectedPawns();
	}

	DragStartTile = nullptr;
}

void ASS_PlayerController::RightMouseButtonPressed()
{
	if (PCMode == ESS_PlayerControllerMode::Selection)
	{
		for (const auto& SelectedPawn : SelectedPawns)
		{
			ASS_Character* CharacterToMove = Cast<ASS_Character>(SelectedPawn);
			if (CharacterToMove)
			{
				CharacterToMove->MoveData = SavedMoveData;
			}
		}
	}
}

void ASS_PlayerController::RightMouseButtonReleased()
{

}

void ASS_PlayerController::MiddleMouseButtonPressed()
{
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	//FInputModeGameAndUI NewInputMode;
	//NewInputMode.SetHideCursorDuringCapture(false);
	//SetInputMode(NewInputMode);

	
	if (CameraPawn)
	{
		CameraPawn->bIsRotationEnabled = true;
	}
}

void ASS_PlayerController::MiddleMouseButtonReleased()
{
	if (CameraPawn)
	{
		CameraPawn->bIsRotationEnabled = false;
	}
}

void ASS_PlayerController::TabKeyPressed()
{
	if (PCMode == ESS_PlayerControllerMode::Spawn)
	{
		PCMode = ESS_PlayerControllerMode::Selection;
		for (const auto& DemoPawn : DemoPawns)
		{
			if (DemoPawn)
				DemoPawn->Destroy();
		}
		DemoPawns.Empty();
		HUDWidget->TogglePawnSelectionPanelBPI(false);
	}
	else
	{
		UnselectAllPawns();
		PCMode = ESS_PlayerControllerMode::Spawn;
		UpdateHoveredTile();
		HUDWidget->TogglePawnSelectionPanelBPI(true);
	}
}

void ASS_PlayerController::ShiftKeyPressed()
{
	/*SelectedTile = HoveredTile;
	bIsBatchSpawnMode = true;
	UpdateHoveredTile();*/
}

void ASS_PlayerController::ShiftKeyReleased()
{
	/*for (const auto& DemoPawn : DemoPawns)
	{
		DemoPawn->Destroy();
	}
	DemoPawns.Empty();
	
	bIsBatchSpawnMode = false;
	UpdateHoveredTile();*/
}

void ASS_PlayerController::SpaceBarKeyPressed()
{
	for (const auto& GamePawn : GamePawns)
	{
		if (GamePawn)
		{
			ASS_PawnAIController* AIController = Cast<ASS_PawnAIController>(GamePawn->GetController());
			if (AIController)
			{
				AIController->bWantsToMove = true;
			}
		}
	}
}

#pragma endregion
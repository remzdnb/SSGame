#include "Player/SS_PlayerController.h"
#include "Player/SS_PlayerState.h"
#include "Player/SS_CameraPawn.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
#include "Pawn/SS_Pawn.h"
#include "Pawn/SS_PawnAIController.h"
#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "UI/SS_MainHUDWidget.h"
// Engine
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ASS_PlayerController::ASS_PlayerController()
{
	bIsSpawnMode = true;
	bIsBatchSpawnMode = false;
	bIsLeftMouseButtonPressed = false;
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

#pragma region +++++ ASS_PlayerController ...

void ASS_PlayerController::SelectCharacterToSpawn(const FName& NewCharacterRowName)
{
	SelectedPawnDataRowName = NewCharacterRowName;
	SelectedPawnData = *GInstance->GetPawnDataFromRow(SelectedPawnDataRowName);
	OnNewSelectedPawnEvent.Broadcast();
}

void ASS_PlayerController::UpdateHoveredTile()
{
	// Clear all previous stuff
	
	for (const auto& DemoPawn : DemoPawns)
	{
		if (DemoPawn)
		{
			DemoPawn->Destroy();
		}
	}
	DemoPawns.Empty();
	Grid->DisableAllHighlight();

	//

	if (HoveredTile.IsValid() == false)
		return;

	if (HoveredTile->TileData.Team != PState->Team)
		return;

    if (bIsSpawnMode)
    {
    	if (bIsBatchSpawnMode)
    	{
    		const TArray<ASS_Tile*> ResultTiles = Grid->GetTilesFromSquare(SelectedTile.Get(), HoveredTile.Get());
    		TArray<FSS_TileGroupData> ValidTileGroups;
    		Grid->GetValidTileGroupsFromSquare(ValidTileGroups, ResultTiles, SelectedPawnData.Size);
    		for (const auto& TileGroup : ValidTileGroups)
    		{
    			DemoPawns.Add(Grid->SpawnPawn(TileGroup, SelectedPawnDataRowName, true));
    		}

    		for (const auto& ResultTile : ResultTiles)
    		{
    			ResultTile->HighlightColumnParticles(true, true);
    			ResultTile->HighlightLineParticles(true, true);
    			Grid->HighlightLine(ResultTile->TileData.YPosition, true);
    		}
    	}
    	else
    	{
    		FSS_TileGroupData DemoPawnTileGroup;
    		Grid->GetSpawnTileGroupFromTile(DemoPawnTileGroup, HoveredTile.Get(), SelectedPawnData.Size, ESS_Team::North);
    		Grid->DebugTileGroup(DemoPawnTileGroup);
    		if (DemoPawnTileGroup.bIsValid &&
				DemoPawnTileGroup.bIsInSpawn &&
				DemoPawnTileGroup.OriginTile->TileData.Team == PState->Team)
    		{
    			DemoPawns.Add(Grid->SpawnPawn(DemoPawnTileGroup, SelectedPawnDataRowName, true));
    		}

    		Grid->HighlightLine(HoveredTile->TileData.YPosition, true);
    		HoveredTile->HighlightColumnParticles(true, true);
    		HoveredTile->HighlightLineParticles(true, true);
    	}
    }

	/*	Grid->HighlightLine(HoveredTile->TileData.YPosition, true);
		HoveredTile->HighlightLineParticles(true);
		
		if (bIsLeftMouseButtonPressed)
		{
			const TArray<ASS_Tile*> ResultTiles = Grid->GetTilesFromSquare(SelectedTile.Get(), HoveredTile.Get());
			TArray<FSS_TileGroupData> ValidTileGroups;

			Grid->GetValidTileGroupsFromSquare(ValidTileGroups, ResultTiles, SelectedPawnData.Size);
			for (const auto& TileGroup : ValidTileGroups)
			{
				DemoPawns.Add(Grid->SpawnPawn(TileGroup, SelectedPawnDataRowName, true));
				//Grid->OnTileHoverBegin(Tile, SelectedPawnRowName);
			}
		}
	}*/

	/*if (HoveredTile.IsValid())
	{
	if (bIsLeftMouseButtonPressed)
	{
	TArray<ASS_Tile*> ResultTiles = Grid->GetTilesFromSquare(SelectedTile.Get(), HoveredTile.Get());

	for (const auto& Tile : ResultTiles)
	{
	FSS_TileGroupData NewTileGroup;
	Grid->GetTileGroup(NewTileGroup, Tile, 1);
	DemoPawns.Add(Grid->SpawnPawn(NewTileGroup, SelectedPawnDataRowName, true));
	//Grid->OnTileHoverBegin(Tile, SelectedPawnRowName);
	}
	}
	}*/
	
	/*if (HoveredActor == HitResult.Actor)
	return;
	
	if (HoveredTile.IsValid())
	Grid->OnTileHoverEnd();

	HoveredActor = HitResult.Actor;

	HoveredTile = Cast<ASS_Tile>(HitResult.Actor);
	if (HoveredTile.IsValid())
	{
	Grid->OnTileHoverBegin(HoveredTile.Get(), SelectedPawnRowName);
	//HoveredTile->OnHoverStart(SelectedPawnRowName);
	}*/
}

#pragma endregion

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
	FInputModeGameAndUI NewInputMode;
	NewInputMode.SetHideCursorDuringCapture(false);
	SetInputMode(NewInputMode);
	
	/*FHitResult HitResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
	ASS_Tile* const HitZone = Cast<ASS_Tile>(HitResult.Actor);
	if (HitZone)
	{
		Grid->OnTileClicked(HitZone, SelectedPawnRowName);
	}*/

	if (HoveredTile.IsValid())
	{
		SelectedTile = HoveredTile;
		bIsLeftMouseButtonPressed = true;
	}
		
}

void ASS_PlayerController::OnLeftMouseButtonReleased()
{
	for (const auto& DemoPawn : DemoPawns)
	{
		GamePawns.Add(Grid->SpawnPawn(DemoPawn->TileGroup, SelectedPawnDataRowName));
	}

	for (const auto& DemoPawn : DemoPawns)
	{
		DemoPawn->Destroy();
	}

	DemoPawns.Empty();
	
	bIsLeftMouseButtonPressed = false;
}

void ASS_PlayerController::RightMouseButtonPressed()
{
	
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
	if (bIsSpawnMode)
	{
		bIsSpawnMode = false;
		for (const auto& DemoPawn : DemoPawns)
		{
			if (DemoPawn)
				DemoPawn->Destroy();
		}
		DemoPawns.Empty();
	}
	else
	{
		bIsSpawnMode = true;
		UpdateHoveredTile();
	}
	
	if (HUDWidget)
		HUDWidget->TogglePawnSelectionPanelBPI(bIsSpawnMode);
}

void ASS_PlayerController::ShiftKeyPressed()
{
	SelectedTile = HoveredTile;
	bIsBatchSpawnMode = true;
	UpdateHoveredTile();
}

void ASS_PlayerController::ShiftKeyReleased()
{
	for (const auto& DemoPawn : DemoPawns)
	{
		DemoPawn->Destroy();
	}
	DemoPawns.Empty();
	
	bIsBatchSpawnMode = false;
	UpdateHoveredTile();
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
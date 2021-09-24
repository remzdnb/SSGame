#include "SS_PlayerController.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
#include "Player/SS_CameraPawn.h"
#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "UI/SS_MainHUDWidget.h"
// Engine
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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

	SelectedPawnRowName = GInstance->PawnDT->GetRowNames()[0];
}

void ASS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this);
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
	HitResultTraceDistance = 1000000.0f;

	// Camera

	for (TActorIterator<ASS_CameraPawn> FoundCameraPawn(GetWorld()); FoundCameraPawn; ++FoundCameraPawn)
	{
		CameraPawn = *FoundCameraPawn;
	}
	if (CameraPawn)
	{
		Possess(CameraPawn);
	}
	
	// UI

	USS_MainHUDWidget* const MainHUDWidget = CreateWidget<USS_MainHUDWidget>(
		GetWorld(),
		GInstance->GameSettings->MainHUD_WBP
	);
	if (MainHUDWidget)
	{
		MainHUDWidget->AddToViewport();
	}
}

void ASS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHoveredActor();
}

void ASS_PlayerController::SelectCharacterToSpawn(const FName& NewCharacterRowName)
{
	SelectedPawnRowName = NewCharacterRowName;

	OnNewSelectedPawnEvent.Broadcast();
}

#pragma region +++++ Input ...

void ASS_PlayerController::UpdateHoveredActor()
{
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

	if (HoveredActor == HitResult.Actor)
		return;
	
	if (HoveredTile.IsValid())
		Grid->OnTileHoverEnd();

	HoveredActor = HitResult.Actor;

	HoveredTile = Cast<ASS_Tile>(HitResult.Actor);
	if (HoveredTile.IsValid())
	{
		Grid->OnTileHoverBegin(HoveredTile.Get(), SelectedPawnRowName);
		//HoveredTile->OnHoverStart(SelectedPawnRowName);
	}
}

void ASS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAxis("LookUpAxis", this, &ASS_PlayerController::LookUpAxis).bConsumeInput = false;
	InputComponent->BindAxis("LookRightAxis", this, &ASS_PlayerController::LookRightAxis).bConsumeInput = false;
	InputComponent->BindAxis("ZoomAxis", this, &ASS_PlayerController::ZoomAxis).bConsumeInput = false;
	InputComponent->BindAxis("MoveForwardAxis", this, &ASS_PlayerController::MoveForwardAxis).bConsumeInput = false;
	InputComponent->BindAxis("MoveRightAxis", this, &ASS_PlayerController::MoveRightAxis).bConsumeInput = false;
	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ASS_PlayerController::OnLeftMouseButtonPressed).bConsumeInput = false;
	InputComponent->BindAction("RightMouseButton", IE_Pressed, this, &ASS_PlayerController::RightMouseButtonPressed).bConsumeInput = false;
	InputComponent->BindAction("RightMouseButton", IE_Released, this, &ASS_PlayerController::RightMouseButtonReleased).bConsumeInput = false;
	InputComponent->BindAction("Shift", IE_Pressed, this, &ASS_PlayerController::ShiftKeyPressed).bConsumeInput = false;
	InputComponent->BindAction("Shift", IE_Released, this, &ASS_PlayerController::ShiftKeyReleased).bConsumeInput = false;
	InputComponent->BindAction("SpaceBar", IE_Pressed, this, &ASS_PlayerController::SpaceBarKeyPressed).bConsumeInput = false;
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
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
	ASS_Tile* const HitZone = Cast<ASS_Tile>(HitResult.Actor);
	if (HitZone)
	{
		Grid->OnTileClicked(HitZone, SelectedPawnRowName);
	}
}

void ASS_PlayerController::RightMouseButtonPressed()
{
	if (CameraPawn)
	{
		CameraPawn->bIsRotationEnabled = true;
	}
}

void ASS_PlayerController::RightMouseButtonReleased()
{
	if (CameraPawn)
	{
		CameraPawn->bIsRotationEnabled = false;
	}
}

void ASS_PlayerController::ShiftKeyPressed()
{
}

void ASS_PlayerController::ShiftKeyReleased()
{
}

void ASS_PlayerController::SpaceBarKeyPressed()
{
}

#pragma endregion
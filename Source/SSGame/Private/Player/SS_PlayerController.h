#pragma once

#include "SSGame.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SS_PlayerController.generated.h"

class USS_GameInstance;
class ASS_PlayerState;
class ASS_CameraPawn;
class USS_MainHUDWidget;
class ASS_Grid;
class ASS_Tile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewSelectedPawnDelegate);

UCLASS()
class ASS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ASS_PlayerController();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Pawn() override;

private:
	
	USS_GameInstance* GInstance;
	ASS_CameraPawn* CameraPawn;
	USS_MainHUDWidget* HUDWidget;
	ASS_PlayerState* PState;
	ASS_Grid* Grid;

	ESS_PlayerControllerMode PCMode;

	///// Pawns
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	FOnNewSelectedPawnDelegate OnNewSelectedPawnEvent;

	//

	UFUNCTION()
	void SelectCharacterToSpawn(const FName& NewCharacterRowName);

	UFUNCTION()
	const FName& GetCharacterToSpawnRowName() const { return SelectedPawnDataRowName; }

private:

	ASS_Tile* HoveredTile;
	ASS_Tile* DragStartTile;
	TArray<ASS_Tile*> SelectedTiles;
	TArray<TWeakObjectPtr<ASS_Pawn>> SelectedPawns;
	FSS_MoveData SavedMoveData;

	//
	
	UPROPERTY()
	FName SelectedPawnDataRowName;

	UPROPERTY()
	FSS_PawnData SelectedPawnData;

	UPROPERTY()
	TArray<ASS_Pawn*> DemoPawns;

	UPROPERTY()
	TArray<ASS_Pawn*> GamePawns;
	
	//
	
	UFUNCTION()
	void UpdateHoveredTile();

	UFUNCTION()
	void UpdateDemoPawns();

	UFUNCTION()
	void UpdateSelectedPawns();

	UFUNCTION()
	void UnselectAllPawns();

	UFUNCTION()
	void UpdateDemoMovement();

	UFUNCTION(Server, Reliable)
	void SpawnPawn(); // query spawn pawn to grid
	
	///// Input
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	
	virtual void SetupInputComponent() override;

private:
	
	UFUNCTION() void LookUpAxis(float AxisValue);
	UFUNCTION() void LookRightAxis(float AxisValue);
	UFUNCTION() void ZoomAxis(float AxisValue);
	UFUNCTION() void MoveForwardAxis(float AxisValue);
	UFUNCTION() void MoveRightAxis(float AxisValue);
	UFUNCTION() void OnLeftMouseButtonPressed();
	UFUNCTION() void OnLeftMouseButtonReleased();
	UFUNCTION() void RightMouseButtonPressed();
	UFUNCTION() void RightMouseButtonReleased();
	UFUNCTION() void MiddleMouseButtonPressed();
	UFUNCTION() void MiddleMouseButtonReleased();
	UFUNCTION() void TabKeyPressed();
	UFUNCTION() void ShiftKeyPressed();
	UFUNCTION() void ShiftKeyReleased();
	UFUNCTION() void SpaceBarKeyPressed();
};

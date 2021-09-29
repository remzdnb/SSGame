#pragma once

#include "SSGame.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SS_PlayerController.generated.h"

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

	// References

private:
	
	class USS_GameInstance* GInstance;
	class ASS_CameraPawn* CameraPawn;
	class USS_MainHUDWidget* HUDWidget;
	class ASS_PlayerState* PState;
	class ASS_Grid* Grid;

	///// Spawn
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	FOnNewSelectedPawnDelegate OnNewSelectedPawnEvent;

	//

	UFUNCTION()
	void SelectCharacterToSpawn(const FName& NewCharacterRowName);

	UFUNCTION()
	const FName& GetCharacterToSpawnRowName() const { return SelectedPawnDataRowName; }

private:

	TWeakObjectPtr<class ASS_Tile> HoveredTile;
	TWeakObjectPtr<class ASS_Tile> SelectedTile;

	//

	UPROPERTY()
	bool bIsSpawnMode;

	UPROPERTY()
	bool bIsBatchSpawnMode;
	
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
	
	///// Input
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	
	virtual void SetupInputComponent() override;

private:

	UPROPERTY()
	bool bIsLeftMouseButtonPressed;

	//

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

#pragma once

#include "SSGame.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SS_PlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewSelectedPawnDelegate);

class USS_GameInstance;
class ASS_CameraPawn;
class ASS_Grid;
class ASS_Tile;
class ASS_Pawn;

UCLASS()
class ASS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ASS_PlayerController();

	// Actor

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// References

private:
	
	USS_GameInstance* GInstance;
	ASS_CameraPawn* CameraPawn;
	ASS_Grid* Grid;

	///// Pawn
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void SelectCharacterToSpawn(const FName& NewCharacterRowName);

	//

	FORCEINLINE UFUNCTION() const FName& GetCharacterToSpawnRowName() const { return SelectedPawnRowName; }

	//

	FOnNewSelectedPawnDelegate OnNewSelectedPawnEvent;

private:

	UPROPERTY()
	FName SelectedPawnRowName;
	
	///// Input
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	
	virtual void SetupInputComponent() override;

	//
	
	UFUNCTION()
	void UpdateHoveredActor();

private:

	UFUNCTION() void LookUpAxis(float AxisValue);
	UFUNCTION() void LookRightAxis(float AxisValue);
	UFUNCTION() void ZoomAxis(float AxisValue);
	UFUNCTION() void MoveForwardAxis(float AxisValue);
	UFUNCTION() void MoveRightAxis(float AxisValue);
	UFUNCTION() void OnLeftMouseButtonPressed();
	UFUNCTION() void RightMouseButtonPressed();
	UFUNCTION() void RightMouseButtonReleased();
	UFUNCTION() void ShiftKeyPressed();
	UFUNCTION() void ShiftKeyReleased();
	UFUNCTION() void SpaceBarKeyPressed();

	//

	TWeakObjectPtr<AActor> HoveredActor;
	TWeakObjectPtr<ASS_Tile> HoveredTile;
	TWeakObjectPtr<ASS_Pawn> HoveredPawn;
	
};

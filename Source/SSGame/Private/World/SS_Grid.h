#pragma once

#include "SSGame.h"
#include "GameFramework/Actor.h"
#include "SS_Grid.generated.h"

class USS_GameInstance;
class ASS_Tile;
class ASS_Pawn;
class UArrowComponent;

UCLASS()
class ASS_Grid : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASS_Grid();

	// Actor
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	
	// References

	USS_GameInstance* GInstance;

	///// Scene Components
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UArrowComponent* DirectionArrowCT;

	///// Tiles
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void OnTileHoverBegin(ASS_Tile* HoveredTile, const FName& SelectedPawnRowName);

	UFUNCTION()
	void OnTileHoverEnd();

	UFUNCTION()
	void OnTileClicked(ASS_Tile* const ClickedTile, const FName& SelectedPawnRowName);

private:

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Editor Tools")
	void SpawnTiles();

	//
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> TileBP;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 GridSizeX;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 GridSizeY;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 BaseSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<class ASS_Tile*> TileArray;

	///// Pawns
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void SpawnPawn(
		const FSS_TileGroupData& TileGroup,
		const FName& CharacterRowName,
		bool bIsDemoPawn = false
	);

	UFUNCTION()
	bool RequestPawnMovement(ASS_Pawn* PawnToMove);
	
	UFUNCTION()
	void RegisterPawnToGrid(ASS_Pawn* PawnToRegister, const FSS_TileGroupData& TileGroup);

	UFUNCTION()
	void UnregisterPawnFromGrid(ASS_Pawn* PawnToUnregister);

private:

	UPROPERTY()
	TArray<ASS_Pawn*> PawnArray;

	UPROPERTY()
	ASS_Pawn* DemoPawn;

	UPROPERTY()
	FSS_TileGroupData DemoPawnTileGroup;

	///// Tools
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	ASS_Tile* const GetTileFromCoords(int32 TileX, int32 TileY) const;

	UFUNCTION()
	void GetTileGroup(
		FSS_TileGroupData& GroupResult,
		ASS_Tile* OriginTile,
		int32 GroupSizeX,
		int32 GroupSizeY,
		const ASS_Pawn* PawnToIgnore = nullptr
	);
	
	UFUNCTION()
	ASS_Tile* GetForwardTile(const ASS_Tile* OriginTile, ESS_Team Team);
};

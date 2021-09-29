#pragma once

#include "SSGame.h"
#include "GameFramework/Actor.h"
#include "SS_Grid.generated.h"

class ASS_Tile;
class ASS_Pawn;
class UArrowComponent;

UCLASS()
class ASS_Grid : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASS_Grid();
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	//

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UArrowComponent* DirectionArrowCT;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UDataTable* GameAIPresetDT;

private:

	class USS_GameInstance* GInstance;

	///// Tiles
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	class ASS_Pawn* SpawnPawn(
		const FSS_TileGroupData& TileGroup,
		const FName& PawnDataRowName,
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

	///// Helpers
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	ASS_Tile* GetTileFromPosition(int32 XPosition, int32 YPosition) const;

	UFUNCTION()
	ASS_Tile* GetForwardTile(const ASS_Tile* OriginTile, ESS_Team Team) const;

	UFUNCTION()
	TArray<ASS_Tile*> GetTilesFromSquare(const ASS_Tile* FirstCornerTile, const ASS_Tile* SecondCornerTile) const;
	
	UFUNCTION()
	void GetTileGroup(FSS_TileGroupData& GroupResult, ASS_Tile* OriginTile, int32 GroupSize,
	                  const ASS_Pawn* PawnToIgnore = nullptr);

	UFUNCTION()
	void GetSpawnTileGroupFromTile(FSS_TileGroupData& GroupResult, ASS_Tile* OriginTile, int32 GroupSize,
	                               ESS_Team Team);

	UFUNCTION()
	void GetValidTileGroupsFromSquare(TArray<FSS_TileGroupData>& GroupsResult, TArray<ASS_Tile*> SquareTiles,
	                                  int32 GroupSize); // from array ?


	UFUNCTION()
	void DebugTileGroup(const FSS_TileGroupData& TileGroup);
	
	///// Rendering
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION()
	void HighlightLine(int32 PositionY, bool bActivate);

	UFUNCTION()
	void DisableAllHighlight();
};

#pragma once

#include "SSGame.h"
#include "GameFramework/Actor.h"
#include "SS_Grid.generated.h"

class ASS_Character;

UCLASS()
class ASS_Grid : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASS_Grid();

	void Init(TSubclassOf<AActor> TileClass, int32 NewGridSizeY, int32 NewGridSizeX, int32 NewSpawnSize);
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	//

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UArrowComponent* DirectionArrowCT;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UDataTable* GameAIPresetDT;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<class ASS_Tile*> TileArray;

	//

	UPROPERTY() int32 GridSizeY;
	UPROPERTY() int32 GridSizeX;
	UPROPERTY() int32 SpawnSize;

private:

	class USS_GameInstance* GInstance;

	///// Pawns
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	class ASS_Pawn* SpawnPawn(
		const FSS_TileGroupData& TileGroup,
		const FName& PawnDataRowName,
		bool bIsDemoPawn = false
	);

	UFUNCTION(Server, Reliable)
	void SpawnPawnServer(class ASS_Tile* OriginTile, const FName& PawnDataRowName, bool bIsDemoPawn = false);

	UFUNCTION()
	bool RequestCharacterMovement(ASS_Character* CharacterToMove, ESS_Direction Direction);
	
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
	
	UFUNCTION() // get closest tile in line better
	ASS_Tile* GetClosestTile(const ASS_Tile* TargetTile, TArray<ASS_Tile*> TilesToCheck) const;

	UFUNCTION()
	void GetMoveData(FSS_MoveData& MoveDataResult, const ASS_Tile* StartTile, const ASS_Tile* TargetTile) const;

	UFUNCTION()
	ASS_Tile* GetTileFromMoveData(const ASS_Tile* OriginTile, const FSS_MoveData& MoveData);

	UFUNCTION()
	ASS_Tile* GetNextTile(const ASS_Tile* OriginTile, ESS_Direction Direction) const;

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

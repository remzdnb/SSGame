#pragma once

#include "SSGame.h"
#include "GameFramework/Info.h"
#include "SS_WorldGenerator.generated.h"

class ASS_Grid;
class ASS_Tile;
class ASS_CameraPawn;
class UPostProcessComponent;


UCLASS(Blueprintable)
class ASS_WorldGenerator : public AInfo
{
	GENERATED_BODY()

	ASS_WorldGenerator();

public:

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* PostProcessCT;
	
	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ASS_Grid> GridClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ASS_Tile> TileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ASS_CameraPawn> CameraPawnClass;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<APostProcessVolume> PostProcessClass;

	//

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 GridSizeY;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 GridSizeX;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 SpawnSize;

	//
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SS_WorldGenerator")
	void RegenerateWorld();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SS_WorldGenerator")
	void ClearWorld();
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SS_GameSettings.generated.h"

UCLASS()
class USS_GameSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	USS_GameSettings();

	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	UCurveFloat* PawnMovementCurveFloat;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	UMaterialInterface* DemoPawnMaterial_Valid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	UMaterialInterface* DemoPawnMaterial_Invalid;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	float TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor AllyTileColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor EnemyTileColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor NeutralTileColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	float EmptyTileOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	float OwnedTileOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	TSubclassOf<AActor> PostProcess_BP;
	
	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MainHUD_WBP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CharacterSelectionItem_WBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PawnOTM_WBP;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<class AAIController> AIControllerClass;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	bool bDebugGrid = false;
};

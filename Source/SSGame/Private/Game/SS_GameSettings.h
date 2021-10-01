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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float StrategicPhaseTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float BattlePhaseTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float BattleEndPhaseTime;
	
	//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    bool bAutoSpawnAIController;
	
	//
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	UMaterialInterface* DemoPawnMaterial_Valid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	UMaterialInterface* DemoPawnMaterial_Invalid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	class UParticleSystem* SpawnParticle;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor TileParticleColor_01;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor TileParticleColor_02;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor TileParticleColor_03;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor AllyTileParticleColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FLinearColor EnemyTileParticleColor;
	
	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MainHUD_WBP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CharacterSelectionItem_WBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PawnOTM_WBP;

	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc")
	UCurveFloat* LinearCurveFloat;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugPawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugProjectile;
};

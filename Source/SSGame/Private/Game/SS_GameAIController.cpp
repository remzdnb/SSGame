#include "Game/SS_GameAIController.h"
#include "Game/SS_GameInstance.h"
#include "Player/SS_PlayerState.h"
#include "World/SS_Grid.h"

ASS_GameAIController::ASS_GameAIController()
{
	bWantsPlayerState = true;
}

void ASS_GameAIController::BeginPlay()
{
	Super::BeginPlay();
	
	GInstance = Cast<USS_GameInstance>(GetGameInstance());
	Grid = GInstance->Grid;
	Cast<ASS_PlayerState>(PlayerState)->Team = ESS_Team::South;

	if (Grid->GameAIPresetDT)
	{
		const FString ContextString;
		const FSS_WaveSpawnData* const NewWaveSpawnData = Grid->GameAIPresetDT->FindRow<FSS_WaveSpawnData>(
			Grid->GameAIPresetDT->GetRowNames()[0],
			ContextString
		);
		if (NewWaveSpawnData)
		{
			WaveSpawnData = *NewWaveSpawnData;
			GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ASS_GameAIController::SpawnWave, WaveSpawnData.NextWaveTimer, true, 0.0f);
		}
	}
}

void ASS_GameAIController::SpawnWave()
{
	for (const auto& PawnSpawnData : WaveSpawnData.PawnsToSpawn)
	{
		const FSS_PawnData* const PawnData = GInstance->GetPawnDataFromRow(PawnSpawnData.PawnDataRowName);
		if (PawnData)
		{
			FSS_TileGroupData PawnTileGroup;
			Grid->GetTileGroup(PawnTileGroup, Grid->GetTileFromPosition(PawnSpawnData.XPosition, PawnSpawnData.YPosition), PawnData->Size);
			if (PawnTileGroup.bIsValid)
			{
				Grid->SpawnPawn(PawnTileGroup, PawnSpawnData.PawnDataRowName);
			}
		}
	}
}

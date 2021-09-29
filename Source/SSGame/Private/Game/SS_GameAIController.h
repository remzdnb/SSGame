#pragma once

#include "SSGame.h"
#include "AIController.h"
#include "SS_GameAIController.generated.h"

UCLASS()
class ASS_GameAIController : public AAIController
{
	GENERATED_BODY()

public:

	ASS_GameAIController();

	virtual void BeginPlay() override;

	//

	class USS_GameInstance* GInstance;
	class ASS_Grid* Grid;
	
	UFUNCTION()
	void SpawnWave();

	FTimerHandle WaveTimerHandle;

	FSS_WaveSpawnData WaveSpawnData;
};

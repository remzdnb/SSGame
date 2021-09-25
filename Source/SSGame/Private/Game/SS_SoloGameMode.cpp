#include "Game/SS_SoloGameMode.h"
#include "Game/SS_GameAIController.h"
//
#include "Kismet/GameplayStatics.h"

ASS_SoloGameMode::ASS_SoloGameMode()
{
}

void ASS_SoloGameMode::BeginPlay()
{
	Super::BeginPlay();

	GameAIController = GetWorld()->SpawnActorDeferred<ASS_GameAIController>(
		ASS_GameAIController::StaticClass(),
		FTransform::Identity,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (GameAIController)
	{
		UGameplayStatics::FinishSpawningActor(GameAIController, FTransform::Identity);
	}
}

void ASS_SoloGameMode::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);
}


#include "Game/SS_GameMode.h"
#include "Game/SS_GameSettings.h"
#include "Game/SS_GameState.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameAIController.h"
#include "Player/SS_PlayerController.h"
#include "Player/SS_PlayerState.h"
//
#include "Kismet/GameplayStatics.h"

ASS_GameMode::ASS_GameMode()
{
	GameStateClass = ASS_GameState::StaticClass();
	PlayerStateClass = ASS_PlayerState::StaticClass();
	PlayerControllerClass = ASS_PlayerController::StaticClass();
	HUDClass = nullptr;
	DefaultPawnClass = nullptr;
	SpectatorClass = nullptr;
}

void ASS_GameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GInstance = Cast<USS_GameInstance>(GetGameInstance());
}

void ASS_GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GInstance->GameSettings->bAutoSpawnAIController)
	{
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
}

void ASS_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ASS_PlayerController* NewPlayerController = Cast<ASS_PlayerController>(NewPlayer);
	if (NewPlayerController)
	{
		if (NorthTeamController == nullptr)
		{
			NorthTeamController = NewPlayerController;
			Cast<ASS_PlayerState>(NewPlayerController->PlayerState)->Team = ESS_Team::North;
		}
		else if (SouthTeamController == nullptr)
		{
			SouthTeamController = NewPlayerController;
			Cast<ASS_PlayerState>(NewPlayerController->PlayerState)->Team = ESS_Team::South;
		}
	}
}

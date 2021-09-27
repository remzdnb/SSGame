#include "Game/SS_GameMode.h"

#include "SS_GameSettings.h"
#include "Game/SS_GameState.h"
#include "Game/SS_GameInstance.h"
#include "Player/SS_PlayerController.h"
#include "Player/SS_PlayerState.h"

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
}

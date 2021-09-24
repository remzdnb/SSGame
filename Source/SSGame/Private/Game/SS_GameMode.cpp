#include "SS_GameMode.h"
#include "Game/SS_GameInstance.h"
#include "Player/SS_PlayerController.h"

ASS_GameMode::ASS_GameMode()
{
	GameStateClass = nullptr;
	PlayerStateClass = nullptr;
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
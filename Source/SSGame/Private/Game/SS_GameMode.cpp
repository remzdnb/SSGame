#include "Game/SS_GameMode.h"
#include "Game/SS_GameSettings.h"
#include "Game/SS_GameState.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameAIController.h"
#include "Player/SS_PlayerController.h"
#include "Player/SS_PlayerState.h"
#include "World/SS_Grid.h"
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
	GState = Cast<ASS_GameState>(GetWorld()->GetGameState());
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

	FSS_TileGroupData SouthKingTileGroup;
	GInstance->Grid->GetTileGroup(
		SouthKingTileGroup,
		GInstance->Grid->GetTileFromPosition(0, 0),
		1
	);
	GInstance->Grid->SpawnPawn(SouthKingTileGroup, "King");

	FSS_TileGroupData NorthKingTileGroup;
	GInstance->Grid->GetTileGroup(
		SouthKingTileGroup,
		GInstance->Grid->GetTileFromPosition(GInstance->Grid->GridSizeX - 1, GInstance->Grid->GridSizeY - 1),
		1
	);
	GInstance->Grid->SpawnPawn(SouthKingTileGroup, "King");
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

	if (SouthTeamController && NorthTeamController)
	{
		GState->SetNewPhaseMulticast(ESS_GamePhase::Ready);
		SetNextPhase();
	}
}
//GetWorldTimerManager().ClearTimer(PhaseTimerHandle);

void ASS_GameMode::SetNextPhase()
{
	if (GState->GetCurrentGamePhase() == ESS_GamePhase::Ready)
	{
		GState->SetNewPhaseMulticast(ESS_GamePhase::Strategic);
		GetWorldTimerManager().SetTimer(
			PhaseTimerHandle,
			this,
			&ASS_GameMode::SetNextPhase,
			GInstance->GameSettings->StrategicPhaseTime,
			false,
			GInstance->GameSettings->StrategicPhaseTime
		);
	}
	else if (GState->GetCurrentGamePhase() == ESS_GamePhase::Strategic)
	{
		GState->SetNewPhaseMulticast(ESS_GamePhase::Battle);
		GetWorldTimerManager().SetTimer(
			PhaseTimerHandle,
			this,
			&ASS_GameMode::SetNextPhase,
			GInstance->GameSettings->BattlePhaseTime,
			false,
			GInstance->GameSettings->BattlePhaseTime
		);
	}
	else if (GState->GetCurrentGamePhase() == ESS_GamePhase::Battle)
	{
		GState->SetNewPhaseMulticast(ESS_GamePhase::BattleEnd);
		GetWorldTimerManager().SetTimer(
			PhaseTimerHandle,
			this,
			&ASS_GameMode::SetNextPhase,
			GInstance->GameSettings->BattleEndPhaseTime,
			false,
			GInstance->GameSettings->BattleEndPhaseTime
		);
	}
	else if (GState->GetCurrentGamePhase() == ESS_GamePhase::BattleEnd)
	{
		GState->SetNewPhaseMulticast(ESS_GamePhase::Strategic);
		GetWorldTimerManager().SetTimer(
			PhaseTimerHandle,
			this,
			&ASS_GameMode::SetNextPhase,
			GInstance->GameSettings->StrategicPhaseTime,
			false,
			GInstance->GameSettings->StrategicPhaseTime
		);
	}
}

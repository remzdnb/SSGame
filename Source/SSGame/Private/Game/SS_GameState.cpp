#include "Game/SS_GameState.h"
//
#include "Net/UnrealNetwork.h"

ASS_GameState::ASS_GameState()
{
	CurrentPhase = ESS_GamePhase::WaitingForPlayers;
}

void ASS_GameState::SetNewPhaseMulticast_Implementation(ESS_GamePhase NewPhase)
{
	CurrentPhase = NewPhase;
	OnGamePhaseUpdatedEvent.Broadcast(CurrentPhase);
}

/*void ASS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASS_GameState, CurrentPhase);
}*/

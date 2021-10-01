#pragma once

#include "SSGame.h"
#include "GameFramework/GameStateBase.h"
#include "SS_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseUpdatedDelegate, ESS_GamePhase, NewPhase);

UCLASS()
class ASS_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ASS_GameState();

	//

	FOnGamePhaseUpdatedDelegate OnGamePhaseUpdatedEvent;

	//
	
	UFUNCTION(NetMulticast, Reliable)
	void SetNewPhaseMulticast(ESS_GamePhase NewPhase);

	UFUNCTION() ESS_GamePhase GetCurrentGamePhase() const { return CurrentPhase; }

private:
	
	UPROPERTY()
	ESS_GamePhase CurrentPhase;
};

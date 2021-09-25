#pragma once

#include "SSGame.h"
#include "GameFramework/PlayerState.h"
#include "SS_PlayerState.generated.h"

UCLASS()
class ASS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ASS_PlayerState();

	UPROPERTY()
	ESS_Team Team;
};

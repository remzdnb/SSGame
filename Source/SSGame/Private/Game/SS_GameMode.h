#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SS_GameMode.generated.h"

class USS_GameInstance;
class ASS_GameState;

UCLASS()
class ASS_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASS_GameMode();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:

	USS_GameInstance* GInstance;
	ASS_GameState* GState;
	
	FTimerHandle PhaseTimerHandle;

	UPROPERTY()
	AController* SouthTeamController;

	UPROPERTY()
	AController* NorthTeamController;
	
	UPROPERTY()
	class ASS_GameAIController* GameAIController;

	//

	UFUNCTION()
	void SetNextPhase();
	
};


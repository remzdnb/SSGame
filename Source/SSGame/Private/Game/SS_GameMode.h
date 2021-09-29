#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SS_GameMode.generated.h"

class USS_GameInstance;

UCLASS()
class ASS_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASS_GameMode();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:

	class USS_GameInstance* GInstance;

	UPROPERTY()
	AController* SouthTeamController;

	UPROPERTY()
	AController* NorthTeamController;
	
	UPROPERTY()
	class ASS_GameAIController* GameAIController;
};

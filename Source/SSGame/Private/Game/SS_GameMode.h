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

protected:

	USS_GameInstance* GInstance;
};

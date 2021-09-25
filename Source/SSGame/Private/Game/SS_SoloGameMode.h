#pragma once

#include "CoreMinimal.h"
#include "Game/SS_GameMode.h"
#include "SS_SoloGameMode.generated.h"

class ASS_GameAIController;
//
class UDataTable;

UCLASS()
class ASS_SoloGameMode : public ASS_GameMode
{
	GENERATED_BODY()

public:
	
	ASS_SoloGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayerController) override;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* WavesConfigDT;

private:

	UPROPERTY()
	ASS_GameAIController* GameAIController;
};

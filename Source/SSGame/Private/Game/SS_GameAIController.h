#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SS_GameAIController.generated.h"

UCLASS()
class ASS_GameAIController : public AAIController
{
	GENERATED_BODY()

public:

	ASS_GameAIController();

	virtual void BeginPlay() override;
};

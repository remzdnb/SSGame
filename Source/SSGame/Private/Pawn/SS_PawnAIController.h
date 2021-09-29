#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SS_PawnAIController.generated.h"

class ASS_Pawn;
class ASS_Grid;

UCLASS()
class ASS_PawnAIController : public AAIController
{
	GENERATED_BODY()

public:

	ASS_PawnAIController();

protected:

	// Actor

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// AController
	
	virtual void OnPossess(APawn* InPawn) override;

	// References

private:

	UFUNCTION()
	void StartNextAction();

	UPROPERTY()
	ASS_Grid* Grid;
	
	UPROPERTY() //Weakptr ?
	ASS_Pawn* ControlledPawn;

	// AI

public:
	
	UPROPERTY()
	bool bWantsToMove;
};

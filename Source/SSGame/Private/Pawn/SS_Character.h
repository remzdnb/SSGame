#pragma once

#include "CoreMinimal.h"
#include "Pawn/SS_Pawn.h"
#include "SS_Character.generated.h"

UCLASS()
class ASS_Character : public ASS_Pawn
{
	GENERATED_BODY()

public:

	ASS_Character();
	
	virtual void Init_Multicast(const FName& PawnDataRowName, const FSS_TileGroupData& InitialTileGroup, ESS_Team NewTeam,
	                  bool bIsDemoPawn = false, bool bIsDemoPawnValidLocation = false) override;
	
	virtual void Tick(float DeltaTime) override;
	
	///// Movement
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	FSS_MoveData MoveData;

	UFUNCTION()
	void StartNewMove(ESS_PawnMoveType NewMoveType);
	
	UFUNCTION()
	void StartMoveToTileGroup();

	UFUNCTION()
	void UpdateMoveArrow(bool bNewIsVisible, ASS_Tile* TargetTile = nullptr);

private:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* MoveArrowCT;

	//

	UPROPERTY()
	UCurveFloat* MoveCurve;

	UPROPERTY()
	FTimeline MoveTimeline;

	UPROPERTY()
	float MoveTimelineProgressValue;

	UPROPERTY()
	FVector MoveStartLocation;

	UPROPERTY()
	FVector MoveDirection;

	UPROPERTY()
	float MoveDistance;

	//

	UFUNCTION()
	void MoveTimelineProgress(float Value);

	UFUNCTION()
	void MoveTimelineEnd();
};

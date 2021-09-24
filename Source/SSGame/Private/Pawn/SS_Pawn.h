#pragma once

#include "SSGame.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "SS_Pawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnStateUpdatedDelegate, ESS_PawnState, NewPawnState);

UCLASS()
class ASS_Pawn : public APawn
{
	GENERATED_BODY()

public:

	// Initialization

	ASS_Pawn();

	UFUNCTION()
	void Init(
		const FName& PawnDataRowName,
		const FSS_TileGroupData& SpawnTileGroup,
		ESS_Team NewTeam = ESS_Team::Neutral,
		bool bIsDemoPawn = false,
		bool bIsDemoPawnValidLocation = false
	);

	// Actor
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// Pawn

	FOnPawnStateUpdatedDelegate OnPawnStateUpdatedEvent;

	UPROPERTY()
	FSS_PawnData PawnData;

	UPROPERTY()
	ESS_Team Team;

	UPROPERTY()
	ESS_PawnState State;

private:

	// Scene Components

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* MeshCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OTMWidgetCT;
	
	// References

	class USS_GameInstance* GInstance;
	class USS_PawnOTMWidget* OTMWidget;

	///// Movement
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void StartMoveToTileGroup();

	UFUNCTION()
	void MovementTimelineProgress(float Value);

	UFUNCTION()
	void MovementTimelineEnd();

	//

	UPROPERTY()
	FSS_TileGroupData TileGroup;

private:

	UPROPERTY()
	UCurveFloat* MovementCurve;

	UPROPERTY()
	FTimeline MovementTimeline;

	UPROPERTY()
	float MovementTimelineProgressValue;

	UPROPERTY()
	FVector MoveStartLocation;
};

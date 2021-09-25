#include "Pawn/SS_Pawn.h"
#include "World/SS_Tile.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
#include "UI/SS_PawnOTMWidget.h"
//
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASS_Pawn::ASS_Pawn()
{
	RootSceneCT = CreateDefaultSubobject<USceneComponent>(FName("RootSceneCT"));
	RootComponent = RootSceneCT;

	MeshCT = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MeshCT"));
	MeshCT->SetCollisionProfileName("NoCollision");
	MeshCT->SetGenerateOverlapEvents(false);
	MeshCT->SetupAttachment(RootComponent);

	CollisionBoxCT = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBoxCT"));
	CollisionBoxCT->SetupAttachment(RootComponent);

	DetectionBoxCT = CreateDefaultSubobject<UBoxComponent>(FName("DetectionBoxCT"));
	DetectionBoxCT->SetupAttachment(RootComponent);

	OTMWidgetCT = CreateDefaultSubobject<UWidgetComponent>(FName("OTMWidgetCT"));
	OTMWidgetCT->SetupAttachment(MeshCT);
	
	PrimaryActorTick.bCanEverTick = true;

	//

	State = ESS_PawnState::Idling;
}

void ASS_Pawn::Init(
	const FName& PawnDataRowName,
	const FSS_TileGroupData& SpawnTileGroup,
	ESS_Team NewTeam,
	bool bIsDemoPawn,
	bool bIsDemoPawnValidLocation)
{
	TileGroup = SpawnTileGroup;
	Team = NewTeam;
	GInstance = Cast<USS_GameInstance>(GetGameInstance());

	const FSS_PawnData* const NewPawnData = GInstance->GetPawnDataFromRow(PawnDataRowName);
	if (NewPawnData)
	{
		PawnData = *NewPawnData;
		
		MeshCT->SetSkeletalMesh(NewPawnData->MeshTemplate);

		// Set correct mesh relative position
		
		if (PawnData.bIsMeshFacingXAxis == false)
		{
			MeshCT->AddRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		}

		for (uint8 YIndex = 1; YIndex < PawnData.SizeY; YIndex++)
		{
			if (Team == ESS_Team::North)
				MeshCT->AddRelativeLocation(FVector(0.0f, TILESIZE / 2 * -1, 0.0f));
			else
				MeshCT->AddRelativeLocation(FVector(0.0f, TILESIZE / 2, 0.0f));
		}

		for (uint8 XIndex = 1; XIndex < PawnData.SizeX; XIndex++)
		{
			//MeshCT->AddRelativeLocation(FVector(TILESIZE / 2 * -1, 0.0f, 0.0f));
			
			if (Team == ESS_Team::North)
				MeshCT->AddRelativeLocation(FVector(TILESIZE / 2, 0.0f, 0.0f));
			else
				MeshCT->AddRelativeLocation(FVector(TILESIZE / 2 * -1, 0.0f, 0.0f));
		}

		//

		if (bIsDemoPawn)
		{
			State = ESS_PawnState::Disabled;
			OTMWidgetCT->DestroyComponent();
			
			if (bIsDemoPawnValidLocation)
				MeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Valid);
			else
				MeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Invalid);
		}
	}
}

inline void ASS_Pawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld()->IsGameWorld() == false)
		return;

	// Initialize collision box.
	
	int32 CollisionBoxXOffset = 0;
	if (Team == ESS_Team::North)
		CollisionBoxXOffset += TILESIZE / 2 * (PawnData.SizeX - 1);
	else
		CollisionBoxXOffset -= TILESIZE / 2 * (PawnData.SizeX - 1);

	int32 CollisionBoxYOffset = 0;
	if (Team == ESS_Team::North)
		CollisionBoxYOffset -= TILESIZE / 2 * (PawnData.SizeY - 1);
	else
		CollisionBoxYOffset += TILESIZE / 2 * (PawnData.SizeY - 1);
		
	CollisionBoxCT->SetBoxExtent(FVector(PawnData.SizeX * TILESIZE / 2, PawnData.SizeY * TILESIZE / 2, TILESIZE / 2));
	CollisionBoxCT->SetRelativeLocation(FVector(CollisionBoxXOffset, CollisionBoxYOffset, TILESIZE / 2));
	
	// Initialize detection box.

	int32 DetectionBoxXOffset = 0;
	if (Team == ESS_Team::North)
		DetectionBoxXOffset += TILESIZE / 2 * (PawnData.SizeX - 1);
	else
		DetectionBoxXOffset -= TILESIZE / 2 * (PawnData.SizeX - 1);

	DetectionBoxCT->SetBoxExtent(FVector(
		PawnData.AttackRange * TILESIZE / 2 - (TILESIZE / 2),
		PawnData.AttackRange * TILESIZE / 2 - (TILESIZE / 2),
		TILESIZE / 4
	));
	DetectionBoxCT->SetRelativeLocation(FVector(CollisionBoxXOffset, CollisionBoxYOffset, TILESIZE / 2));
}

void ASS_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (OTMWidgetCT)
	{
		OTMWidget = Cast<USS_PawnOTMWidget>(OTMWidgetCT->GetWidget());
		if (OTMWidget)
			OTMWidget->Init(this);
	}
	
	MovementCurve = GInstance->GameSettings->PawnMovementCurveFloat;
	if (MovementCurve)
	{
		FOnTimelineFloat MovementTimelineProgressCallback;
		FOnTimelineEventStatic MovementTimelineFinishedCallback;
		MovementTimelineProgressCallback.BindUFunction(this, FName("MovementTimelineProgress"));
		MovementTimelineFinishedCallback.BindUFunction(this, FName("MovementTimelineEnd"));
		MovementTimeline.AddInterpFloat(MovementCurve, MovementTimelineProgressCallback);
		MovementTimeline.SetTimelineFinishedFunc(MovementTimelineFinishedCallback);
		MovementTimeline.SetLooping(false);
		MovementTimeline.SetPlayRate(PawnData.MoveSpeed);
	}
}

void ASS_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementTimeline.TickTimeline(DeltaTime);
}

void ASS_Pawn::StartMoveToTileGroup()
{
	MoveStartLocation = GetActorLocation();
	MoveDirection = UKismetMathLibrary::FindLookAtRotation(
		MoveStartLocation,
		TileGroup.OriginTile->CenterLocation
	).Vector();
	MoveDistance = FVector::Dist(MoveStartLocation, TileGroup.OriginTile->CenterLocation);
	
	MovementTimeline.PlayFromStart();
	State = ESS_PawnState::Moving;
}

void ASS_Pawn::MovementTimelineProgress(float Value)
{
	const FVector NewLocation = MoveStartLocation + MoveDirection * MoveDistance * Value;
	
	SetActorLocation(NewLocation);
}

void ASS_Pawn::MovementTimelineEnd()
{
	State = ESS_PawnState::Idling;
}

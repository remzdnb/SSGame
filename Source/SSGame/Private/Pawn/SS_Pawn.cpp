#include "Pawn/SS_Pawn.h"
#include "World/SS_Tile.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
#include "UI/SS_PawnOTMWidget.h"
//
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

ASS_Pawn::ASS_Pawn()
{
	RootSceneCT = CreateDefaultSubobject<USceneComponent>(FName("RootSceneCT"));
	RootComponent = RootSceneCT;

	MeshCT = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MeshCT"));
	MeshCT->SetCollisionProfileName("NoCollision");
	MeshCT->SetGenerateOverlapEvents(false);
	MeshCT->SetupAttachment(RootComponent);

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
			MeshCT->AddRelativeLocation(FVector(0.0f, TILESIZE / 2, 0.0f));
		}

		for (uint8 XIndex = 1; XIndex < PawnData.SizeX; XIndex++)
		{
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
		MovementTimeline.SetPlayRate(0.5f);
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
	MovementTimeline.PlayFromStart();
	State = ESS_PawnState::Moving;
}

void ASS_Pawn::MovementTimelineProgress(float Value)
{
	const float NewXPosition = MoveStartLocation.X - TileGroup.OriginTile->GetActorLocation().X * Value;
	const float NewYPosition = MoveStartLocation.Y - TileGroup.OriginTile->GetActorLocation().Y * Value;

	//SetActorLocation(FVector(NewXPosition, NewYPosition, GetActorLocation().Z));

	SetActorLocation(TileGroup.OriginTile->CenterLocation);
}

void ASS_Pawn::MovementTimelineEnd()
{
	State = ESS_PawnState::Idling;
}

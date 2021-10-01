#include "World/SS_WorldGenerator.h"
#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "Player/SS_CameraPawn.h"
//
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"

ASS_WorldGenerator::ASS_WorldGenerator()
{
	PostProcessCT = CreateDefaultSubobject<UPostProcessComponent>(FName("PostProcessCT"));
	PostProcessCT->SetupAttachment(RootComponent);
}

void ASS_WorldGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorLocation(FVector(0.0f, 0.0f, 1000.0f));
}

void ASS_WorldGenerator::RegenerateWorld()
{

	ClearWorld();
	
	// Spawn grid

	ASS_Grid* NewGrid = GetWorld()->SpawnActorDeferred<ASS_Grid>(
		GridClass,
		FTransform::Identity,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (NewGrid)
	{
		UGameplayStatics::FinishSpawningActor(NewGrid, FTransform::Identity);
		NewGrid->Init(TileClass, GridSizeY, GridSizeX, SpawnSize);
	}

	// Spawn cameras

	const FVector SouthCameraSpawnLocation = FVector(
		SpawnSize * TILESIZE / 2,
		GridSizeY * TILESIZE / 2, 0.0f
	);
	const FTransform SouthCameraSpawnTransform = FTransform(
		FRotator::ZeroRotator,
		SouthCameraSpawnLocation,
		FVector(1.0f)
	);
	
	ASS_CameraPawn* SouthCameraPawn = GetWorld()->SpawnActorDeferred<ASS_CameraPawn>(
		CameraPawnClass,
		SouthCameraSpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (SouthCameraPawn)
	{
		SouthCameraPawn->Team = ESS_Team::South;
		UGameplayStatics::FinishSpawningActor(SouthCameraPawn, SouthCameraSpawnTransform);
	}

	const FVector NorthCameraSpawnLocation = FVector(
		GridSizeX * TILESIZE - SpawnSize * TILESIZE / 2,
		GridSizeY * TILESIZE / 2, 0.0f
	);
	const FTransform NorthCameraSpawnTransform = FTransform(
		FRotator(0.0f, 180.0f, 0.0f),
		NorthCameraSpawnLocation,
		FVector(1.0f)
	);
	
	ASS_CameraPawn* NorthCameraPawn = GetWorld()->SpawnActorDeferred<ASS_CameraPawn>(
		CameraPawnClass,
		NorthCameraSpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (NorthCameraPawn)
	{
		NorthCameraPawn->Team = ESS_Team::North;
		UGameplayStatics::FinishSpawningActor(NorthCameraPawn, NorthCameraSpawnTransform);
	}
}

void ASS_WorldGenerator::ClearWorld()
{
	for (TActorIterator<ASS_Grid> Grid(GetWorld()); Grid; ++Grid)
	{
		Grid->Destroy();
	}

	for (TActorIterator<ASS_Tile> Tile(GetWorld()); Tile; ++Tile)
	{
		Tile->Destroy();
	}

	for (TActorIterator<ASS_CameraPawn> CameraPawn(GetWorld()); CameraPawn; ++CameraPawn)
	{
		CameraPawn->Destroy();
	}
}

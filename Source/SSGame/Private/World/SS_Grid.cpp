#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
//
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

ASS_Grid::ASS_Grid()
{
	DirectionArrowCT = CreateDefaultSubobject<UArrowComponent>(FName("DirectionArrowCT"));
	DirectionArrowCT->SetupAttachment(RootComponent);
	
	PrimaryActorTick.bCanEverTick = false;
}

void ASS_Grid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);
}

void ASS_Grid::BeginPlay()
{
	Super::BeginPlay();

	GInstance = Cast<USS_GameInstance>(GetGameInstance());
}

void ASS_Grid::SpawnTiles()
{
	// Destroy current layout.
	
	for (const auto& Tile : TileArray)
	{
		if (Tile)
		{
			Tile->Destroy();
		}
	}
	
	TileArray.Empty();

	// Spawn new layout.

	for (int32 XIndex = 0; XIndex < GridSizeX; XIndex++)
	{
		for (int32 YIndex = 0; YIndex < GridSizeY; YIndex++)
		{
			const FVector SpawnLocation = FVector(
				GetActorLocation().X + XIndex * TILESIZE,
				GetActorLocation().Y + YIndex * TILESIZE,
				GetActorLocation().Z
			);
			const FTransform SpawnTransform = FTransform(GetActorRotation(), SpawnLocation, FVector(1.0f));

			ASS_Tile* const NewTile = GetWorld()->SpawnActorDeferred<ASS_Tile>(
				TileBP, SpawnTransform,
				this,
				nullptr,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			);
			if (NewTile)
			{
				NewTile->TileData.PositionX = XIndex;
				NewTile->TileData.PositionY = YIndex;
				if (XIndex < BaseSize)
				{
					NewTile->TileData.Type = ESS_TileType::Spawn;
					NewTile->TileData.Team = ESS_Team::South;
					NewTile->TileData.DefaultSpawnRotationYaw = GetActorRotation().Yaw;
				}
				else if (XIndex >= GridSizeX - BaseSize)
				{
					NewTile->TileData.Type = ESS_TileType::Spawn;
					NewTile->TileData.Team = ESS_Team::North;
					NewTile->TileData.DefaultSpawnRotationYaw = GetActorRotation().Yaw + 180.0f;
				}
				else
				{
					NewTile->TileData.Type = ESS_TileType::Base;
					NewTile->TileData.Team = ESS_Team::Neutral;
				}

				TileArray.Add(NewTile);
				UGameplayStatics::FinishSpawningActor(NewTile, SpawnTransform);
			}
		}
	}
}

void ASS_Grid::OnTileHoverBegin(ASS_Tile* HoveredTile, const FName& SelectedPawnRowName)
{
	if (HoveredTile == nullptr)
		return;
	
	const FSS_PawnData* PawnData = GInstance->GetPawnDataFromRow(SelectedPawnRowName);
	if (PawnData)
	{
		GetTileGroup(DemoPawnTileGroup, HoveredTile, PawnData->Size);
		SpawnPawn(DemoPawnTileGroup, SelectedPawnRowName, true);
	}
}

void ASS_Grid::OnTileHoverEnd()
{
	if (DemoPawn)
	{
		DemoPawn->Destroy();
	}
}

void ASS_Grid::OnTileClicked(ASS_Tile* const ClickedTile, const FName& SelectedPawnRowName)
{
	if (DemoPawnTileGroup.bIsValid && DemoPawnTileGroup.bIsInSpawn)
	{
		SpawnPawn(DemoPawnTileGroup, SelectedPawnRowName, false);
	}
}

void ASS_Grid::SpawnPawn(
	const FSS_TileGroupData& TileGroup,
	const FName& PawnDataRowName,
	bool bIsDemoPawn
)
{
	const FSS_PawnData* const PawnData = GInstance->GetPawnDataFromRow(PawnDataRowName);
	if (PawnData == nullptr)
		return;
	
	//
	
	const FVector SpawnLocation = FVector(
		TileGroup.OriginTile->GetActorLocation().X + TILESIZE / 2,
		TileGroup.OriginTile->GetActorLocation().Y + TILESIZE / 2,
		0.0f
	);

	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
	/*if (TileGroup.OriginTile->TileData.Team == ESS_Team::South)
		SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
	if (TileGroup.OriginTile->TileData.Team == ESS_Team::North)
		SpawnRotation = FRotator(0.0f, 180.0f, 0.0f);*/
	
	const FTransform SpawnTransform = FTransform(
		SpawnRotation,
		SpawnLocation,
		FVector(1.0f)
	);

	//
	
	ASS_Pawn* NewPawn = GetWorld()->SpawnActorDeferred<ASS_Pawn>(
		PawnData->Class,
		SpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (NewPawn)
	{
		bool bIsValidDemoPawn = true;
		if (TileGroup.bIsValid == false || TileGroup.bIsInSpawn == false)
			bIsValidDemoPawn = false;
		
		NewPawn->Init(
			PawnDataRowName,
			TileGroup,
			TileGroup.OriginTile->TileData.Team,
			bIsDemoPawn,
			bIsValidDemoPawn
		);

		if (bIsDemoPawn)
		{
			DemoPawn = NewPawn;
		}
		else
		{
			RegisterPawnToGrid(NewPawn, TileGroup);
			PawnArray.Add(NewPawn);
		}
		
		UGameplayStatics::FinishSpawningActor(NewPawn, SpawnTransform);
	}
}

bool ASS_Grid::RequestPawnMovement(ASS_Pawn* PawnToMove)
{
	ASS_Tile* ForwardTile = GetForwardTile(PawnToMove->TileGroup.OriginTile, PawnToMove->Team); // pass direction instead of team
	if (ForwardTile == nullptr)
		return false;
	
	FSS_TileGroupData TargetTileGroup;
	GetTileGroup(
		TargetTileGroup,
		ForwardTile,
		PawnToMove->PawnData.Size,
		PawnToMove
	);
	if (TargetTileGroup.bIsValid)
	{
		UnregisterPawnFromGrid(PawnToMove);
		RegisterPawnToGrid(PawnToMove, TargetTileGroup);
		PawnToMove->StartMoveToTileGroup();

		UE_LOG(LogTemp, Display, TEXT("ASS_Grid::RequestPawnMovement // Valid : TargetTileX = %s // TargetTileY = %s"),
		       *FString::FromInt(TargetTileGroup.OriginTile->TileData.PositionX),
		       *FString::FromInt(TargetTileGroup.OriginTile->TileData.PositionY)
		);
	}
	
	return TargetTileGroup.bIsValid;
}

void ASS_Grid::RegisterPawnToGrid(ASS_Pawn* PawnToRegister, const FSS_TileGroupData& TileGroup)
{
	UE_LOG(LogTemp, Display, TEXT("ASS_Grid::RegisterPawnToGrid"));
		
	for (const auto& Tile : TileGroup.TileArray)
	{
		Tile->RegisterPawnBPI(PawnToRegister);
		PawnToRegister->TileGroup.TileArray.Add(Tile);
	}

	PawnToRegister->TileGroup.OriginTile = TileGroup.OriginTile;
}

void ASS_Grid::UnregisterPawnFromGrid(ASS_Pawn* PawnToUnregister)
{
	UE_LOG(LogTemp, Display, TEXT("ASS_Grid::UnregisterPawnFromGrid"));
	
	for (const auto& Tile : PawnToUnregister->TileGroup.TileArray)
	{
		Tile->UnregisterPawnBPI();
	}

	PawnToUnregister->TileGroup.OriginTile = nullptr;
	PawnToUnregister->TileGroup.TileArray.Empty();
}

ASS_Tile* const ASS_Grid::GetTileFromCoords(int32 TileX, int32 TileY) const
{
	if (TileX >= GridSizeX || TileY >= GridSizeY)
		return nullptr;

	const int32 TileIndex = GridSizeY * TileX + TileY;

	/*UE_LOG(LogTemp, Display, TEXT("ASS_Grid::GetTileFromCoords : TileX = %s , TileY = %s , TileIndex = %s"),
	       *FString::FromInt(TileX),
	       *FString::FromInt(TileY),
	       *FString::FromInt(TileIndex)
	);*/

	if (TileArray.IsValidIndex(TileIndex))
	{
		return TileArray[TileIndex];	
	}
	else
	{
		return nullptr;	
	}
}

void ASS_Grid::GetTileGroup(
	FSS_TileGroupData& GroupResult,
	ASS_Tile* OriginTile,
	int32 GroupSize,
	const ASS_Pawn* PawnToIgnore)
{
	GroupResult.bIsValid = true;
	GroupResult.bIsInSpawn = true;
	GroupResult.TileArray.Empty();
	
	if (OriginTile == nullptr)
	{
		GroupResult.bIsValid = false;
		return;
	}

	GroupResult.OriginTile = OriginTile;

	for (int32 XOffset = 0; XOffset < GroupSize; XOffset++)
	{
		for (int32 YOffset = 0; YOffset < GroupSize; YOffset++)
		{
			ASS_Tile* TileToAdd = GetTileFromCoords(
				OriginTile->TileData.PositionX + XOffset,
				OriginTile->TileData.PositionY + YOffset
			);

			if (TileToAdd == nullptr)
			{
				GroupResult.bIsValid = false;
				return;
			}

			if (TileToAdd->TileData.Type != ESS_TileType::Spawn)
			{
				GroupResult.bIsInSpawn = false;
			}

			if (TileToAdd->GetRegisteredPawn() != nullptr)
			{
				if (PawnToIgnore == nullptr)
				{
					GroupResult.bIsValid = false;
					return;
				}
				
				if (TileToAdd->GetRegisteredPawn() != PawnToIgnore)
				{
					GroupResult.bIsValid = false;
					return;
				}
			}

			GroupResult.TileArray.Add(TileToAdd);
		}
	}
}

ASS_Tile* ASS_Grid::GetForwardTile(const ASS_Tile* OriginTile, ESS_Team Team)
{
	if (Team == ESS_Team::South)
	{
		return (GetTileFromCoords(OriginTile->TileData.PositionX + 1, OriginTile->TileData.PositionY));
	}
	
	if (Team == ESS_Team::North)
	{
		return (GetTileFromCoords(OriginTile->TileData.PositionX - 1, OriginTile->TileData.PositionY));
	}
	
	return nullptr;
}


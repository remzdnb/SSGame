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

void ASS_Grid::OnTileHoverBegin(ASS_Tile* const HoveredTile, const FName& SelectedPawnRowName)
{
	const FSS_PawnData* PawnData = GInstance->GetPawnDataFromRow(SelectedPawnRowName);
	if (PawnData)
	{
		DemoPawnTileGroup = MakeSpawnTileGroup(HoveredTile, PawnData->SizeX, PawnData->SizeY);
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
	if (DemoPawnTileGroup.bIsValid)
	{
		SpawnPawn(DemoPawnTileGroup, SelectedPawnRowName, false);
	}
}

void ASS_Grid::SpawnPawn(
	const FSS_TileGroupData& TileGroup,
	const FName& CharacterRowName,
	bool bIsDemoPawn
)
{
	const FVector SpawnLocation = FVector(
		TileGroup.OriginTile->GetActorLocation().X + TILESIZE / 2,
		TileGroup.OriginTile->GetActorLocation().Y + TILESIZE / 2,
		0.0f
	);
	const FTransform SpawnTransform = FTransform(
		FRotator(0.0f, 0.0f, 0.0f),
		SpawnLocation,
		FVector(1.0f)
	);
	
	ASS_Pawn* NewPawn = GetWorld()->SpawnActorDeferred<ASS_Pawn>(
		GInstance->GameSettings->PawnBP, SpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (NewPawn)
	{
		NewPawn->Init(
			CharacterRowName,
			TileGroup,
			TileGroup.OriginTile->TileData.Team,
			bIsDemoPawn,
			TileGroup.bIsValid
		);
		
		UGameplayStatics::FinishSpawningActor(NewPawn, SpawnTransform);
		
		if (bIsDemoPawn)
		{
			DemoPawn = NewPawn;
		}
		else
		{
			RegisterPawnToGrid(NewPawn, TileGroup);
			PawnArray.Add(NewPawn);
		}
	}
}

bool ASS_Grid::RequestPawnMovement(ASS_Pawn* PawnToMove)
{
	ASS_Tile* ForwardTile = GetForwardTile(PawnToMove->TileGroup.OriginTile, PawnToMove->Team);
	if (ForwardTile == nullptr)
		return false;
	
	FSS_TileGroupData TargetTileGroup;
	const bool bIsValidTargetTileGroup = GetTileGroup(
		TargetTileGroup,
		ForwardTile,
		PawnToMove->PawnData.SizeX,
		PawnToMove->PawnData.SizeY,
		PawnToMove
	);
	if (bIsValidTargetTileGroup)
	{
		UnregisterPawnFromGrid(PawnToMove);
		RegisterPawnToGrid(PawnToMove, TargetTileGroup);
		PawnToMove->StartMoveToTileGroup();
	}

	UE_LOG(LogTemp, Display, TEXT("ASS_Grid::RequestPawnMovement : ValidTargetTileGroup = "),
	       *FString::FromInt(bIsValidTargetTileGroup)
	);

	return bIsValidTargetTileGroup;
}

void ASS_Grid::RegisterPawnToGrid(ASS_Pawn* PawnToRegister, const FSS_TileGroupData& TileGroup)
{
	for (const auto& Tile : TileGroup.TileArray)
	{
		Tile->RegisterPawnBPI(PawnToRegister);
		PawnToRegister->TileGroup.TileArray.Add(Tile);
	}

	PawnToRegister->TileGroup.OriginTile = TileGroup.OriginTile;
}

void ASS_Grid::UnregisterPawnFromGrid(ASS_Pawn* PawnToUnregister)
{
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

/*TArray<ASS_Tile*> ASS_Grid::GetTileGroup(const ASS_Tile* OriginTile, int32 GroupSizeX, int32 GroupSizeY) const
{
	TArray<ASS_Tile*> TileGroup;
	
	for (int32 XOffset = 0; XOffset > GroupSizeX * -1; XOffset--)
	{
		for (int32 YOffset = 0; YOffset < GroupSizeY; YOffset++)
		{
			TileGroup.Add(GetTileFromCoords(
				OriginTile->TileData.PositionX + XOffset,
				OriginTile->TileData.PositionY + YOffset
			));
		}
	}
	
	return TileGroup;
}*/

FSS_TileGroupData ASS_Grid::MakeSpawnTileGroup(ASS_Tile* OriginTile, int32 GroupSizeX, int32 GroupSizeY) const
{
	FSS_TileGroupData TileGroup;

	TileGroup.OriginTile = OriginTile;

	for (int32 XOffset = 0; XOffset > GroupSizeX * -1; XOffset--)
	{
		for (int32 YOffset = 0; YOffset < GroupSizeY; YOffset++)
		{
			ASS_Tile* TileToAdd = GetTileFromCoords(
				OriginTile->TileData.PositionX + XOffset,
				OriginTile->TileData.PositionY + YOffset
			);

			if (TileToAdd == nullptr ||
				TileToAdd->TileData.Type != ESS_TileType::Spawn ||
				TileToAdd->GetRegisteredPawn() != nullptr)
			{
				TileGroup.bIsValid = false;
				return TileGroup;
			}

			TileGroup.TileArray.Add(TileToAdd);
		}
	}
	
	return TileGroup;
}

bool ASS_Grid::GetTileGroup(
	FSS_TileGroupData& GroupResult,
	ASS_Tile* OriginTile,
	int32 GroupSizeX,
	int32 GroupSizeY,
	const ASS_Pawn* PawnToIgnore)
{
	if (OriginTile == nullptr)
		return false; // ToDo : check better solution for crash
	
	GroupResult.OriginTile = OriginTile;

	for (int32 XOffset = 0; XOffset > GroupSizeX * -1; XOffset--)
	{
		for (int32 YOffset = 0; YOffset < GroupSizeY; YOffset++)
		{
			ASS_Tile* TileToAdd = GetTileFromCoords(
				OriginTile->TileData.PositionX + XOffset,
				OriginTile->TileData.PositionY + YOffset
			);

			if (TileToAdd->TileData.Type != ESS_TileType::Spawn)
			{
				GroupResult.bIsInSpawn = false;
			}

			if (TileToAdd->GetRegisteredPawn() != nullptr)
			{
				if (PawnToIgnore == nullptr)
				{
					return false;
				}
				
				if (TileToAdd->GetRegisteredPawn() != PawnToIgnore)
				{
					return false;
				}
			}

			GroupResult.TileArray.Add(TileToAdd);
		}
	}
	
	return true;
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

bool ASS_Grid::IsValidPawnLocation(const ASS_Tile* const OriginTile, const FName& PawnRowName) const
{
	const FSS_PawnData* const PawnData = GInstance->GetPawnDataFromRow(PawnRowName);
	if (PawnData == nullptr)
		return false;

	if (OriginTile->GetRegisteredPawn().IsValid())
	{
		return false;
	}
	
	for (uint8 XIndex = 1; XIndex < PawnData->SizeX; XIndex++)
	{
	}
	
	return true;
}


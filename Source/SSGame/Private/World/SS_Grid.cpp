#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "Pawn/SS_Character.h"
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

void ASS_Grid::Init(TSubclassOf<AActor> TileClass, int32 NewGridSizeY, int32 NewGridSizeX, int32 NewSpawnSize)
{
	GridSizeY = NewGridSizeY;
	GridSizeX = NewGridSizeX;
	SpawnSize = NewSpawnSize;
	
	for (int32 XPosition = 0; XPosition < GridSizeX; XPosition++)
	{
		for (int32 YPosition = 0; YPosition < GridSizeY; YPosition++)
		{
			const FVector SpawnLocation = FVector(
				GetActorLocation().X + XPosition * TILESIZE,
				GetActorLocation().Y + YPosition * TILESIZE,
				GetActorLocation().Z
			);
			const FTransform SpawnTransform = FTransform(GetActorRotation(), SpawnLocation, FVector(1.0f));

			ASS_Tile* const NewTile = GetWorld()->SpawnActorDeferred<ASS_Tile>(
				TileClass, SpawnTransform,
				this,
				nullptr,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			);
			if (NewTile)
			{
				NewTile->TileData.XPosition = XPosition;
				NewTile->TileData.YPosition = YPosition;
				if (XPosition < SpawnSize)
				{
					NewTile->TileData.Type = ESS_TileType::Spawn;
					NewTile->TileData.Team = ESS_Team::South;
				}
				else if (XPosition >= GridSizeX - SpawnSize)
				{
					NewTile->TileData.Type = ESS_TileType::Spawn;
					NewTile->TileData.Team = ESS_Team::North;
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

ASS_Pawn* ASS_Grid::SpawnPawn(
	const FSS_TileGroupData& TileGroup,
	const FName& PawnDataRowName,
	bool bIsDemoPawn
)
{
	const FSS_PawnData* const PawnData = GInstance->GetPawnDataFromRow(PawnDataRowName);
	if (PawnData == nullptr)
		return nullptr;
	
	//
	
	const FVector SpawnLocation = FVector(
		TileGroup.OriginTile->GetActorLocation().X + TILESIZE / 2,
		TileGroup.OriginTile->GetActorLocation().Y + TILESIZE / 2,
		0.0f
	);
	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, SpawnLocation, FVector(1.0f));
	
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
		
		NewPawn->Init_Multicast(
			PawnDataRowName,
			TileGroup,
			TileGroup.OriginTile->TileData.Team,
			bIsDemoPawn,
			bIsValidDemoPawn
		);

		if (bIsDemoPawn == false)
		{
			RegisterPawnToGrid(NewPawn, TileGroup);
			PawnArray.Add(NewPawn);
		}
		
		UGameplayStatics::FinishSpawningActor(NewPawn, SpawnTransform);
	}

	return NewPawn;
}

void ASS_Grid::SpawnPawnServer_Implementation(ASS_Tile* OriginTile, const FName& PawnDataRowName, bool bIsDemoPawn)
{
	const FSS_PawnData* const PawnData = GInstance->GetPawnDataFromRow(PawnDataRowName);
	if (PawnData == nullptr)
		return;
	
	FSS_TileGroupData SpawnTileGroup;
	GetTileGroup(SpawnTileGroup, OriginTile, PawnData->Size);
	if (SpawnTileGroup.bIsValid == false || SpawnTileGroup.bIsInSpawn == false)
		return;
	
	const FVector SpawnLocation = FVector(
		SpawnTileGroup.OriginTile->GetActorLocation().X + TILESIZE / 2,
		SpawnTileGroup.OriginTile->GetActorLocation().Y + TILESIZE / 2,
		0.0f
	);
	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, SpawnLocation, FVector(1.0f));
	
	ASS_Pawn* NewPawn = GetWorld()->SpawnActorDeferred<ASS_Pawn>(
		PawnData->Class,
		SpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (NewPawn)
	{
		/*bool bIsValidDemoPawn = true;
		if (TileGroup.bIsValid == false || TileGroup.bIsInSpawn == false)
			bIsValidDemoPawn = false;*/
		
		NewPawn->Init_Multicast(
			PawnDataRowName,
			SpawnTileGroup,
			SpawnTileGroup.OriginTile->TileData.Team,
			bIsDemoPawn,
			true
		);

		if (bIsDemoPawn == false)
		{
			RegisterPawnToGrid(NewPawn, SpawnTileGroup);
			PawnArray.Add(NewPawn);
		}
		
		UGameplayStatics::FinishSpawningActor(NewPawn, SpawnTransform);
	}
}

bool ASS_Grid::RequestCharacterMovement(ASS_Character* CharacterToMove, ESS_Direction Direction)
{
	ASS_Tile* NextTile = GetNextTile(CharacterToMove->TileGroup.OriginTile, Direction);
	if (NextTile == nullptr)
		return false;
	
	FSS_TileGroupData TargetTileGroup;
	GetTileGroup(
		TargetTileGroup,
		NextTile,
		CharacterToMove->PawnData.Size,
		CharacterToMove
	);
	if (TargetTileGroup.bIsValid)
	{
		UnregisterPawnFromGrid(CharacterToMove);
		RegisterPawnToGrid(CharacterToMove, TargetTileGroup);
		CharacterToMove->StartMoveToTileGroup();

		/*UE_LOG(LogTemp, Display, TEXT("ASS_Grid::RequestPawnMovement // Valid : TargetTileX = %s // TargetTileY = %s"),
		       *FString::FromInt(TargetTileGroup.OriginTile->TileData.XPosition),
		       *FString::FromInt(TargetTileGroup.OriginTile->TileData.YPosition)
		);*/
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

#pragma region +++++ Helpers ...

ASS_Tile* ASS_Grid::GetTileFromPosition(int32 XPosition, int32 YPosition) const
{
	if (XPosition >= GridSizeX || YPosition >= GridSizeY)
		return nullptr;

	const int32 TileIndex = GridSizeY * XPosition + YPosition;
	if (TileArray.IsValidIndex(TileIndex))
	{
		return TileArray[TileIndex];	
	}

	return nullptr;	
	
	/*UE_LOG(LogTemp, Display, TEXT("ASS_Grid::GetTileFromCoords : TileX = %s , TileY = %s , TileIndex = %s"),
	*FString::FromInt(XPosition),
	*FString::FromInt(YPosition),
	*FString::FromInt(TileIndex)
	);*/
}

ASS_Tile* ASS_Grid::GetClosestTile(const ASS_Tile* TargetTile, TArray<ASS_Tile*> TilesToCheck) const
{
	float MinDistance = 999999999999999999.0f;
	ASS_Tile* ResultTile = nullptr;

	for (const auto& Tile : TilesToCheck)
	{
		const float NewDistance = FVector::Dist(Tile->GetActorLocation(), TargetTile->GetActorLocation());
		if (NewDistance < MinDistance)
		{
			MinDistance = NewDistance;
			ResultTile = Tile;
		}
	}
	
	return ResultTile;
}

void ASS_Grid::GetMoveData(FSS_MoveData& MoveDataResult, const ASS_Tile* StartTile, const ASS_Tile* TargetTile) const
{
	if ((StartTile->TileData.XPosition == TargetTile->TileData.XPosition &&
			StartTile->TileData.YPosition == TargetTile->TileData.YPosition) ||
		(StartTile->TileData.XPosition != TargetTile->TileData.XPosition &&
			StartTile->TileData.YPosition != TargetTile->TileData.YPosition))
	{
		MoveDataResult.bIsValid = false;
		return;
	}
	
	if (StartTile->TileData.XPosition != TargetTile->TileData.XPosition)
	{
		StartTile->TileData.XPosition < TargetTile->TileData.XPosition
			? MoveDataResult.Direction = ESS_Direction::North
			: MoveDataResult.Direction = ESS_Direction::South;

		MoveDataResult.Length = FMath::Abs(StartTile->TileData.XPosition - TargetTile->TileData.XPosition);
	}
	else
	{
		StartTile->TileData.YPosition < TargetTile->TileData.YPosition
			? MoveDataResult.Direction = ESS_Direction::East
			: MoveDataResult.Direction = ESS_Direction::West;

		MoveDataResult.Length = FMath::Abs(StartTile->TileData.YPosition - TargetTile->TileData.YPosition);
	}
}

ASS_Tile* ASS_Grid::GetTileFromMoveData(const ASS_Tile* OriginTile, const FSS_MoveData& MoveData)
{
	if (MoveData.Direction == ESS_Direction::North)
	{
		return GetTileFromPosition(OriginTile->TileData.XPosition + MoveData.Length, OriginTile->TileData.YPosition);
	}
	if (MoveData.Direction == ESS_Direction::South)
	{
		return GetTileFromPosition(OriginTile->TileData.XPosition - MoveData.Length, OriginTile->TileData.YPosition);
	}
	if (MoveData.Direction == ESS_Direction::East)
	{
		return GetTileFromPosition(OriginTile->TileData.XPosition, OriginTile->TileData.YPosition + MoveData.Length);
	}
	if (MoveData.Direction == ESS_Direction::West)
	{
		return GetTileFromPosition(OriginTile->TileData.XPosition, OriginTile->TileData.YPosition - MoveData.Length);
	}

	return nullptr;
}

ASS_Tile* ASS_Grid::GetNextTile(const ASS_Tile* OriginTile, ESS_Direction Direction) const
{
	switch (Direction)
	{
		case ESS_Direction::North:
			return (GetTileFromPosition(OriginTile->TileData.XPosition + 1, OriginTile->TileData.YPosition));
		case ESS_Direction::South:
			return (GetTileFromPosition(OriginTile->TileData.XPosition - 1, OriginTile->TileData.YPosition));
		case ESS_Direction::East:
			return (GetTileFromPosition(OriginTile->TileData.XPosition, OriginTile->TileData.YPosition + 1));
		case ESS_Direction::West:
			return (GetTileFromPosition(OriginTile->TileData.XPosition, OriginTile->TileData.YPosition - 1));
		default:
			return GetTileFromPosition(0, 0);
	}
}

TArray<ASS_Tile*> ASS_Grid::GetTilesFromSquare(const ASS_Tile* FirstCornerTile, const ASS_Tile* SecondCornerTile) const
{
	TArray<ASS_Tile*> ResultArray;
	
	int32 XPositionMin;
	int32 XPositionMax;
	int32 YPositionMin;
	int32 YPositionMax;

	FirstCornerTile->TileData.YPosition <= SecondCornerTile->TileData.YPosition
		? YPositionMin = FirstCornerTile->TileData.YPosition
		: YPositionMin = SecondCornerTile->TileData.YPosition;

	FirstCornerTile->TileData.YPosition >= SecondCornerTile->TileData.YPosition
		? YPositionMax = FirstCornerTile->TileData.YPosition
		: YPositionMax = SecondCornerTile->TileData.YPosition;

	FirstCornerTile->TileData.XPosition <= SecondCornerTile->TileData.XPosition
		? XPositionMin = FirstCornerTile->TileData.XPosition
		: XPositionMin = SecondCornerTile->TileData.XPosition;

	FirstCornerTile->TileData.XPosition >= SecondCornerTile->TileData.XPosition
		? XPositionMax = FirstCornerTile->TileData.XPosition
		: XPositionMax = SecondCornerTile->TileData.XPosition;

	for (int32 YPosition = YPositionMin; YPosition <= YPositionMax; YPosition++)
	{
		for (int32 XPosition = XPositionMin; XPosition <= XPositionMax; XPosition++)
		{
			ResultArray.Add(GetTileFromPosition(XPosition, YPosition));
		}
	}
	
	return ResultArray;
}

void ASS_Grid::GetTileGroup(FSS_TileGroupData& GroupResult, ASS_Tile* OriginTile, int32 GroupSize,
                            const ASS_Pawn* PawnToIgnore)
{
	GroupResult.bIsValid = true;
	GroupResult.bIsInSpawn = true;
	GroupResult.OriginTile = OriginTile;
	GroupResult.TileArray.Empty();
	
	if (OriginTile == nullptr)
	{
		GroupResult.bIsValid = false;
		return;
	}
	
	for (int32 XOffset = 0; XOffset < GroupSize; XOffset++)
	{
		for (int32 YOffset = 0; YOffset < GroupSize; YOffset++)
		{
			ASS_Tile* TileToAdd = GetTileFromPosition(
				OriginTile->TileData.XPosition + XOffset,
				OriginTile->TileData.YPosition + YOffset
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

void ASS_Grid::GetSpawnTileGroupFromTile(FSS_TileGroupData& GroupResult, ASS_Tile* OriginTile, int32 GroupSize,
                                         ESS_Team Team)
{
	TArray<FSS_TileGroupData> GroupResults;
	GroupResult.bIsValid = false;
	GroupResult.bIsInSpawn = false;

	const float MinYPosition = OriginTile->TileData.YPosition - GroupSize + 1;
	const float MinXPosition = OriginTile->TileData.XPosition - GroupSize + 1;
	
	for (int32 YIndex = MinYPosition; YIndex <=  OriginTile->TileData.YPosition; YIndex++)
	{
		for (int32 XIndex = MinXPosition; XIndex <= OriginTile->TileData.XPosition; XIndex++)
		{
			ASS_Tile* OffsettedOriginTile = GetTileFromPosition(XIndex, YIndex);
			if (OffsettedOriginTile)
			{
				FSS_TileGroupData GroupData;
				GetTileGroup(GroupData, OffsettedOriginTile, GroupSize);
				if (GroupData.bIsValid && GroupData.bIsInSpawn)
				{
					GroupResult = GroupData;
					GroupResults.Add(GroupResult);
				}
			}
		}
	}

	/*int32 MaxPositionX = GroupResults[];
	float MinPositionY = 0;
	for (const auto& ValidGroup : GroupResults)
	{
		
	}
	
	// If south team, get outer northwest tile, if north team, get outer southeast, so the origin tile will be in
	// priority the upper left corner when facing enemy team.
	if (Team == ESS_Team::North)*/
}

void ASS_Grid::GetValidTileGroupsFromSquare(TArray<FSS_TileGroupData>& GroupsResult, TArray<ASS_Tile*> SquareTiles,
                                            int32 GroupSize)
{
	GroupsResult.Empty();
	TArray<ASS_Tile*> UsedTiles;
	
	for (const auto& SquareTile : SquareTiles)
	{
		if (UsedTiles.Contains(SquareTile) == false)
		{
			FSS_TileGroupData TileGroup;
			GetTileGroup(TileGroup, SquareTile, GroupSize);
			if (TileGroup.bIsValid)
			{
				bool bContainsSquareTiles = true;
				for (const auto& GroupTile1 : TileGroup.TileArray)
				{
					if (SquareTiles.Contains(GroupTile1) == false || UsedTiles.Contains(GroupTile1))
					{
						bContainsSquareTiles = false;
					}
				}

				if (bContainsSquareTiles)
				{
					GroupsResult.Add(TileGroup);

					for (const auto& GroupTile2 : TileGroup.TileArray)
					{
						UsedTiles.Add(GroupTile2);
					}
				}
			}
		}
	}
}

void ASS_Grid::DebugTileGroup(const FSS_TileGroupData& TileGroup)
{
	UE_LOG(LogTemp, Display, TEXT("-------------------------------------------------------------"));
	UE_LOG(LogTemp, Display, TEXT("bIsValid = %s"), *FString::FromInt(TileGroup.bIsValid));
	UE_LOG(LogTemp, Display, TEXT("bIsInSpawn = %s"), *FString::FromInt(TileGroup.bIsInSpawn));
	int32 Index = 0;
	for (const auto& Tile : TileGroup.TileArray)
	{
		UE_LOG(LogTemp, Display, TEXT("Tile[%s] : XPosition = %s - YPosition = %s"),
		       *FString::FromInt(Index),
		       *FString::FromInt(Tile->TileData.XPosition),
		       *FString::FromInt(Tile->TileData.YPosition)
		);

		Index++;
	}
	UE_LOG(LogTemp, Display, TEXT("-------------------------------------------------------------"));
}

#pragma endregion

#pragma region +++++ Rendering ...

void ASS_Grid::HighlightLine(int32 PositionY, bool bActivate)
{
	for (const auto& Tile : TileArray)
	{
		if (Tile->TileData.YPosition == PositionY)
		{
			Tile->HighlightColumnParticles(bActivate);
		}
	}
}

void ASS_Grid::DisableAllHighlight()
{
	for (const auto& Tile : TileArray)
	{
		Tile->HighlightColumnParticles(false);
		Tile->HighlightLineParticles(false);
	}
}

#pragma endregion


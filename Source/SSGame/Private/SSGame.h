#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SSGame.generated.h"

#define TILESIZE 250.0f

UENUM(BlueprintType)
enum class ESS_Team : uint8
{
	South,
	North,
	Neutral,
	Spectator
};

UENUM(BlueprintType)
enum class ESS_TileType : uint8
{
	Base,
	Spawn
};

UENUM(BlueprintType)
enum class ESS_TileState : uint8
{
	Disabled,
	Empty,
	Registered,
	Owned
};

UENUM(BlueprintType)
enum class ESS_PawnState : uint8
{
	Disabled,
	Idling,
	Moving,
	Attacking
};

USTRUCT(BlueprintType)
struct FSS_TileData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	ESS_TileType Type;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	ESS_Team Team;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int32 PositionX;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int32 PositionY;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float DefaultSpawnRotationYaw;
	
	FSS_TileData()
	{
		Type = ESS_TileType::Base;
		Team = ESS_Team::Neutral;
		PositionX = 0;
		PositionY = 0;
		DefaultSpawnRotationYaw = 0.0f;
	}
};

USTRUCT(BlueprintType)
struct FSS_TileGroupData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsValid;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsInSpawn;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	class ASS_Tile* OriginTile;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	TArray<class ASS_Tile*> TileArray;
	
	FSS_TileGroupData()
	{
		bIsValid = true;
		bIsInSpawn = true;
		OriginTile = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSS_PawnData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMesh* MeshTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsMeshFacingXAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 SizeX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 SizeY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MoveSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AttackRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Owning Pawn", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* IdleAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Owning Pawn", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* MoveAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Owning Pawn", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AnimMontage;
	
	FSS_PawnData()
	{
		DisplayName = "DefaultName";
		MeshTemplate = nullptr;
		bIsMeshFacingXAxis = false;
		SizeX = 1;
		SizeY = 1;
		MoveSpeed = 100.0f;
		AttackRange = 1;
		AttackSpeed = 0.1f;
		IdleAnim = nullptr;
		MoveAnim = nullptr;
		AnimMontage = nullptr;
	}
};

// Game AI

USTRUCT(BlueprintType)
struct FSS_PawnSpawnData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName PawnDataRowName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 PositionX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 PositionY;
	
	FSS_PawnSpawnData()
	{
		PawnDataRowName = "None";
		PositionX = 0;
		PositionY = 0;
	}
};

USTRUCT(BlueprintType)
struct FSS_WaveSpawnData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FSS_PawnSpawnData> WaveSpawnSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NextWaveTimer;
	
	FSS_WaveSpawnData()
	{
		NextWaveTimer = 10.0f;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Conversions
	
template<typename TEnum>
static FORCEINLINE FString GetEnumAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}
	FString Str = enumPtr->GetNameByValue((int64)Value).ToString();
	Str.RemoveAt(0, Name.Len() + 2, true);
	return Str;
}
	
static FORCEINLINE FString GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
{
	//Round to integral if have something like 1.9999 within precision
	float Rounded = roundf(TheFloat);
	if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
	{
		TheFloat = Rounded;
	}
	FNumberFormattingOptions NumberFormat;					//Text.h
	NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
	return FText::AsNumber(TheFloat, &NumberFormat).ToString();
}
	
static FORCEINLINE FText GetFloatAsTextWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
{
	//Round to integral if have something like 1.9999 within precision
	float Rounded = roundf(TheFloat);
	if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
	{
		TheFloat = Rounded;
	}
	FNumberFormattingOptions NumberFormat;					//Text.h
	NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
	return FText::AsNumber(TheFloat, &NumberFormat);
}
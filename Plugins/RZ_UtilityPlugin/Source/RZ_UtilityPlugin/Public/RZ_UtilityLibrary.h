#pragma once

#include "CoreMinimal.h"

class RZ_UTILITYPLUGIN_API RZ_UtilityLibrary
{
	
public:

	RZ_UtilityLibrary();

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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///// Math

	static bool CompareFloats(float Value1, float Value2, float Acceptance);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///// Actor

	static TArray<AActor*> OrderActorsByDistance(TArray<AActor*> ActorsToOrder, FVector TargetLocation);
	static AActor* GetClosestActorFromLocation(TArray<AActor*>, FVector TargetLocation);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///// Debug

	static void PrintBoolToScreen(bool bBoolToPrint, FString PrefixString = "", float TimeToDisplay = 0.1f);
	static void PrintStringToScreen(const FString& PrefixString, const FString& InputString, FColor Color = FColor::Red, int32 Key = -1, float TimeToDisplay = 0.1f);
	static void PrintFloatToScreen(const FString& PrefixString, float InputFloat, FColor Color = FColor::Red, int32 Key = 1, float TimeToDisplay = 0.1f);
	static void PrintVectorToScreen(const FString& PrefixString, FVector InputVector, FColor Color = FColor::Red, int32 Key = 1, float TimeToDisplay = 1.0f);
	static void PrintRotatorToScreen(FRotator RotatorToPrint, FString PrefixString = "", int32 Key = -1, float TimeToDisplay = 0.1f);
	
};

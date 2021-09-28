#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SS_CameraPawn.generated.h"

USTRUCT(BlueprintType)
struct FSS_CameraPawnSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinArmLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxArmLength;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmLengthStep;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LerpSpeed;
	
	FSS_CameraPawnSettings()
	{
		MinArmLength = 200.0f;
		MaxArmLength = 6000.0f;
		ArmLengthStep = 200.0f;
		DefaultPitch = -25.0f;
		MinPitch = -89.0f;
		MaxPitch = 0.0f;
		MoveSpeed = 1000.0f;
		LerpSpeed = 10.0f;
	}
};

UCLASS()
class SSGAME_API ASS_CameraPawn : public APawn
{
	GENERATED_BODY()

public:
	
	ASS_CameraPawn();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//

	UFUNCTION() void MoveForward(float AxisValue);
	UFUNCTION() void MoveRight(float AxisValue);
	UFUNCTION() void AddPitchInput(float AxisValue);
	UFUNCTION() void AddYawInput(float AxisValue);
	UFUNCTION() void AddZoomInput(bool bZoomIn);

	//

	UPROPERTY()
	bool bIsRotationEnabled;

private:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraCT;

	//

	UPROPERTY() FSS_CameraPawnSettings CPSettings;
	UPROPERTY() FRotator TargetArmRotation;
	UPROPERTY() float TargetArmLength;
};

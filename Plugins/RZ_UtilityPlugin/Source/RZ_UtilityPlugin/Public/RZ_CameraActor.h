#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RZ_CameraActor.generated.h"

#define LERPSPEED 5.0f

UENUM(BlueprintType)
enum class ERZ_CameraActorMode : uint8
{
	Free,
	Attached
};

USTRUCT(BlueprintType)
struct FRZ_CameraActorSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinArmLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxArmLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultArmLength_Free;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultArmLength_Attached;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmLengthStep;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed_Free;
	
	FRZ_CameraActorSettings()
	{
		MinArmLength = 10000.0f;
		MaxArmLength = 200000.0f;
		DefaultArmLength_Free = 100000.0f;
		DefaultArmLength_Attached = 25000.0f;
		ArmLengthStep = 10000.0f;
		DefaultPitch = -25.0f;
		MinPitch = -89.0f;
		MaxPitch = 89.0f;
		MoveSpeed_Free = 50000.0f;
	}
};

UCLASS()
class RZ_UTILITYPLUGIN_API ARZ_CameraActor : public AActor
{
	GENERATED_BODY()
	
public:	

	ARZ_CameraActor();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//

	UFUNCTION() void MoveForward(float AxisValue);
	UFUNCTION() void MoveRight(float AxisValue);
	UFUNCTION() void AddPitchInput(float AxisValue);
	UFUNCTION() void AddYawInput(float AxisValue);
	UFUNCTION() void AddZoomInput(bool bZoomIn);

	UFUNCTION() void SetNewTargetActor(const AActor* const NewTargetActor, FVector RelativeOffset = FVector::ZeroVector);
	UFUNCTION() void SetNewTargetComponent(USceneComponent* NewTargetComponent);
	UFUNCTION() void SetNewLocation(const FVector& NewLocation, bool bWithLerp);
	UFUNCTION() void SetNewRotation(const FRotator& NewRotation, bool bWithLerp);
	UFUNCTION() void SetNewArmLength(float NewArmLength, bool bWithLerp);
	
	//
	
	UPROPERTY() FRZ_CameraActorSettings CASettings;
	UPROPERTY() ERZ_CameraActorMode CAMode;
	UPROPERTY() const AActor* TargetActor;
	UPROPERTY() USceneComponent* TargetComponent;
	UPROPERTY() FVector TargetLocation;
	UPROPERTY() FRotator TargetArmRotation;
	UPROPERTY() float TargetArmLength;

private:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootScene;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
};

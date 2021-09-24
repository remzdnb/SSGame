#include "RZ_CameraActor.h"
//
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ARZ_CameraActor::ARZ_CameraActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(FName("RootScene"));
	RootComponent = RootScene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(RootScene);
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);

	//

	CAMode = ERZ_CameraActorMode::Free;
}

void ARZ_CameraActor::BeginPlay()
{
	Super::BeginPlay();

	SpringArm->TargetArmLength = CASettings.DefaultArmLength_Free;
	TargetArmLength = CASettings.DefaultArmLength_Free;
	SpringArm->SetWorldRotation(FRotator(CASettings.DefaultPitch, 0.0f, 0.0f));
	TargetArmRotation = FRotator(CASettings.DefaultPitch, 0.0f, 0.0f);
}

void ARZ_CameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
	}
	else if (TargetComponent)
	{
		TargetLocation = TargetComponent->GetComponentLocation();
	}
	
	SetActorLocation(TargetLocation);
	
	/*if (SpringArm->GetComponentRotation() != TargetArmRotation)
	{
		SpringArm->SetWorldRotation(FMath::Lerp(SpringArm->GetComponentRotation(), TargetArmRotation,
		                                        LERPSPEED * DeltaTime));
	}*/

	if (SpringArm->TargetArmLength != TargetArmLength)
	{
		SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, TargetArmLength, LERPSPEED * DeltaTime);
	}
}

void ARZ_CameraActor::MoveForward(float AxisValue)
{
	if (CAMode != ERZ_CameraActorMode::Free)
		return;
	
	const FRotator Rotation = SpringArm->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	SetActorLocation(GetActorLocation() + Direction * AxisValue * CASettings.MoveSpeed_Free * GetWorld()->GetDeltaSeconds());
}

void ARZ_CameraActor::MoveRight(float AxisValue)
{
	if (CAMode != ERZ_CameraActorMode::Free)
		return;
	
	const FRotator Rotation = SpringArm->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	SetActorLocation(GetActorLocation() + Direction * AxisValue * CASettings.MoveSpeed_Free * GetWorld()->GetDeltaSeconds());
}

void ARZ_CameraActor::AddPitchInput(float AxisValue)
{
	if (SpringArm == nullptr || AxisValue == 0)
		return;

	const float NewPitch = SpringArm->GetComponentRotation().Pitch + (AxisValue * GetWorld()->GetDeltaSeconds() * 50.0f);
	
	if (NewPitch > CASettings.MaxPitch || NewPitch < CASettings.MinPitch)
		return;

	SpringArm->SetWorldRotation(FRotator(
		NewPitch,
		SpringArm->GetComponentRotation().Yaw,
		SpringArm->GetComponentRotation().Roll
	));
	
	TargetArmRotation = FRotator(
		NewPitch,
		TargetArmRotation.Yaw,
		TargetArmRotation.Roll
	);
}

void ARZ_CameraActor::AddYawInput(float AxisValue)
{
	if (SpringArm == nullptr || AxisValue == 0)
		return;

	const float NewYaw = SpringArm->GetComponentRotation().Yaw + (AxisValue * GetWorld()->GetDeltaSeconds() * 50.0f);

	SpringArm->SetWorldRotation(FRotator(
		SpringArm->GetComponentRotation().Pitch,
		NewYaw,
		SpringArm->GetComponentRotation().Roll));

	TargetArmRotation = FRotator(
		TargetArmRotation.Pitch,
		NewYaw,
		TargetArmRotation.Roll
	);
}

void ARZ_CameraActor::AddZoomInput(bool bZoomIn)
{
	TargetArmLength = bZoomIn
		                  ? FMath::Clamp(TargetArmLength - CASettings.ArmLengthStep, CASettings.MinArmLength, CASettings.MaxArmLength)
		                  : FMath::Clamp(TargetArmLength + CASettings.ArmLengthStep, CASettings.MinArmLength, CASettings.MaxArmLength);
}

void ARZ_CameraActor::SetNewTargetActor(const AActor* const NewTargetActor, FVector RelativeOffset)
{
	TargetComponent = nullptr;
	TargetActor = NewTargetActor;
	SpringArm->SetWorldRotation(
		FRotator(
			SpringArm->GetComponentRotation().Pitch,
			NewTargetActor->GetActorRotation().Yaw,
			SpringArm->GetComponentRotation().Roll
		)
	);
}

void ARZ_CameraActor::SetNewTargetComponent(USceneComponent* NewTargetComponent)
{
	TargetActor = nullptr;
	TargetComponent = NewTargetComponent;
}

void ARZ_CameraActor::SetNewLocation(const FVector& NewLocation, bool bWithLerp)
{
	TargetLocation = NewLocation;

	if (bWithLerp == false)
		SetActorLocation(NewLocation);
}

void ARZ_CameraActor::SetNewRotation(const FRotator& NewRotation, bool bWithLerp)
{
	TargetArmRotation = NewRotation;

	if (bWithLerp == false)
		SpringArm->SetWorldRotation(NewRotation);
}

void ARZ_CameraActor::SetNewArmLength(float NewArmLength, bool bWithLerp)
{
	TargetArmLength = NewArmLength;

	if (bWithLerp == false)
		SpringArm->TargetArmLength = NewArmLength;
}

#include "SS_CameraPawn.h"
//
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ASS_CameraPawn::ASS_CameraPawn()
{
	RootSceneCT = CreateDefaultSubobject<USceneComponent>(FName("RootSceneCT"));
	RootComponent = RootSceneCT;

	SpringArmCT = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmCT"));
	SpringArmCT->SetupAttachment(RootSceneCT);
	SpringArmCT->bDoCollisionTest = false;

	CameraCT = CreateDefaultSubobject<UCameraComponent>(FName("CameraCT"));
	CameraCT->SetupAttachment(SpringArmCT);

	//
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	//

	bIsRotationEnabled = false;
	SpringArmCT->SetWorldRotation(FRotator(CPSettings.DefaultPitch, 0.0f, 0.0f));
	TargetArmRotation = FRotator(CPSettings.DefaultPitch, 0.0f, 0.0f);
}

void ASS_CameraPawn::BeginPlay()
{
	Super::BeginPlay();

	TargetArmLength = SpringArmCT->TargetArmLength;
}

void ASS_CameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpringArmCT->TargetArmLength != TargetArmLength)
	{
		SpringArmCT->TargetArmLength = FMath::Lerp(SpringArmCT->TargetArmLength, TargetArmLength, CPSettings.LerpSpeed * DeltaTime);
	}
}

void ASS_CameraPawn::MoveForward(float AxisValue)
{
	const FRotator Rotation = SpringArmCT->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	SetActorLocation(GetActorLocation() + Direction * AxisValue * CPSettings.MoveSpeed * GetWorld()->GetDeltaSeconds());
}

void ASS_CameraPawn::MoveRight(float AxisValue)
{
	const FRotator Rotation = SpringArmCT->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	SetActorLocation(GetActorLocation() + Direction * AxisValue * CPSettings.MoveSpeed * GetWorld()->GetDeltaSeconds());
}

void ASS_CameraPawn::AddPitchInput(float AxisValue)
{
	if (AxisValue == 0 || bIsRotationEnabled == false)
		return;

	const float NewPitch = SpringArmCT->GetComponentRotation().Pitch + (AxisValue * GetWorld()->GetDeltaSeconds() * 50.0f);
	
	if (NewPitch > CPSettings.MaxPitch || NewPitch < CPSettings.MinPitch)
		return;

	SpringArmCT->SetWorldRotation(FRotator(
		NewPitch,
		SpringArmCT->GetComponentRotation().Yaw,
		SpringArmCT->GetComponentRotation().Roll
	));
	
	TargetArmRotation = FRotator(
		NewPitch,
		TargetArmRotation.Yaw,
		TargetArmRotation.Roll
	);
}

void ASS_CameraPawn::AddYawInput(float AxisValue)
{
	if (AxisValue == 0 || bIsRotationEnabled == false)
		return;

	const float NewYaw = SpringArmCT->GetComponentRotation().Yaw + (AxisValue * GetWorld()->GetDeltaSeconds() * 50.0f);

	SpringArmCT->SetWorldRotation(FRotator(
		SpringArmCT->GetComponentRotation().Pitch,
		NewYaw,
		SpringArmCT->GetComponentRotation().Roll));

	TargetArmRotation = FRotator(
		TargetArmRotation.Pitch,
		NewYaw,
		TargetArmRotation.Roll
	);
}

void ASS_CameraPawn::AddZoomInput(bool bZoomIn)
{
	TargetArmLength = bZoomIn
					? FMath::Clamp(TargetArmLength - CPSettings.ArmLengthStep, CPSettings.MinArmLength, CPSettings.MaxArmLength)
					: FMath::Clamp(TargetArmLength + CPSettings.ArmLengthStep, CPSettings.MinArmLength, CPSettings.MaxArmLength);
}

#include "Weapon/SS_ProjectileSpline.h"
#include "Weapon/SS_Projectile.h"
#include "Pawn/SS_Pawn.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
//
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ASS_ProjectileSpline::ASS_ProjectileSpline()
{
	RootSceneCT = CreateDefaultSubobject<USceneComponent>(FName("RootSceneCT"));
	RootComponent = RootSceneCT;
	
	SplineCT = CreateDefaultSubobject<USplineComponent>(FName("SplineCT"));
	SplineCT->SetupAttachment(RootSceneCT);
	
	PrimaryActorTick.bCanEverTick = true;
}

void ASS_ProjectileSpline::Init(const FSS_PawnData& NewOwningPawnData, class ASS_Pawn* NewTargetPawn, class ASS_Projectile* LoadedProjectile)
{
	OwningPawnData = NewOwningPawnData;
	TargetPawn = NewTargetPawn;
	Projectile = LoadedProjectile;

	Projectile->Init(this, NewTargetPawn);
	Projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ASS_ProjectileSpline::BeginPlay()
{
	Super::BeginPlay();

	GInstance = Cast<USS_GameInstance>(GetGameInstance());

	// Setup first spline point.

	SplineCT->SetLocationAtSplinePoint(
		0,
		Projectile->GetActorLocation(),
		ESplineCoordinateSpace::World,
		true
	);

	// Setup mid-air spline point.

	const FVector DirectionToTarget = UKismetMathLibrary::FindLookAtRotation(
		GetOwner()->GetActorLocation(),
		TargetPawn->GetActorLocation()
	).Vector();
	const float DistanceToTarget = FVector::Dist(GetOwner()->GetActorLocation(), TargetPawn->GetActorLocation());
	const FVector EquidistantLocationToTarget = GetActorLocation() + DirectionToTarget * DistanceToTarget / 2;
	const FVector MidAirSplinePointLocation = FVector(
		EquidistantLocationToTarget.X,
		EquidistantLocationToTarget.Y,
		500.0f
	);
	
	SplineCT->SetLocationAtSplinePoint(
		1,
		MidAirSplinePointLocation,
		ESplineCoordinateSpace::World,
		true
	);

	// Setup target spline point.
	
	FSplinePoint TargetSplinePoint;
	TargetSplinePoint.InputKey = 2;
	SplineCT->AddPoint(TargetSplinePoint, true);

	// Spawn & setup projectile.

	/*Projectile = GetWorld()->SpawnActorDeferred<ASS_Projectile>(
		OwningPawnData.ProjectileClass,
		FTransform::Identity,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (Projectile)
	{
		UGameplayStatics::FinishSpawningActor(Projectile, FTransform::Identity);
	}*/

	// Setup timeline.

	ProjectileTimelineCurve = GInstance->GameSettings->LinearCurveFloat;
	if (ProjectileTimelineCurve)
	{
		FOnTimelineFloat ProjectileTimelineProgressCallback;
		FOnTimelineEventStatic ProjectileTimelineFinishedCallback;
		ProjectileTimelineProgressCallback.BindUFunction(this, FName("OnProjectileTimelineProgress"));
		ProjectileTimelineFinishedCallback.BindUFunction(this, FName("OnProjectileTimelineEnd"));
		ProjectileTimeline.AddInterpFloat(ProjectileTimelineCurve, ProjectileTimelineProgressCallback);
		ProjectileTimeline.SetTimelineFinishedFunc(ProjectileTimelineFinishedCallback);
		ProjectileTimeline.SetLooping(false);
		ProjectileTimeline.SetPlayRate(1.0f);
	}

	ProjectileTimeline.PlayFromStart();
}

void ASS_ProjectileSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SplineLength = SplineCT->GetDistanceAlongSplineAtSplinePoint(2);

	ProjectileTimeline.TickTimeline(DeltaTime);

	if (TargetPawn.IsValid())
	{
		SplineCT->SetLocationAtSplinePoint(2, TargetPawn->GetMesh()->GetSocketLocation("TargetSocket"), ESplineCoordinateSpace::World, true);
	}
}

void ASS_ProjectileSpline::OnProjectileTimelineProgress(float NewProgressValue)
{
	const float CurrentDistanceAlongSpline = SplineLength * NewProgressValue;

	Projectile->SetActorLocation(SplineCT->GetLocationAtDistanceAlongSpline(
		CurrentDistanceAlongSpline,
		ESplineCoordinateSpace::World
	));
	Projectile->SetActorRotation(SplineCT->GetRotationAtDistanceAlongSpline(
		CurrentDistanceAlongSpline,
		ESplineCoordinateSpace::World
	));
}

void ASS_ProjectileSpline::OnProjectileTimelineEnd()
{
	if (TargetPawn.IsValid())
	{
		TargetPawn->ReceiveDamage(OwningPawnData.RangedAttackDamage, Projectile);
	}
	else
	{
		Projectile->Destroy();
	}

	this->Destroy();
}





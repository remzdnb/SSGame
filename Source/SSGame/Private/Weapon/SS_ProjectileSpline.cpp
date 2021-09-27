#include "Weapon/SS_ProjectileSpline.h"
#include "Weapon/SS_Projectile.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
//
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

ASS_ProjectileSpline::ASS_ProjectileSpline()
{
	RootSceneCT = CreateDefaultSubobject<USceneComponent>(FName("RootSceneCT"));
	RootComponent = RootSceneCT;
	
	SplineCT = CreateDefaultSubobject<USplineComponent>(FName("SplineCT"));
	SplineCT->SetupAttachment(RootSceneCT);
	
	PrimaryActorTick.bCanEverTick = true;
}

void ASS_ProjectileSpline::Init(const FSS_PawnData& NewOwningPawnData, AActor* NewTargetActor)
{
	OwningPawnData = NewOwningPawnData;
	TargetActor = NewTargetActor;
}

void ASS_ProjectileSpline::BeginPlay()
{
	Super::BeginPlay();

	GInstance = Cast<USS_GameInstance>(GetGameInstance());

	// Setup spline.
	
	FSplinePoint EndSplinePoint;
	EndSplinePoint.InputKey = 2;
	SplineCT->AddPoint(EndSplinePoint, true);

	SplineCT->SetLocationAtSplinePoint(
		1,
		FVector(GetActorLocation().X, GetActorLocation().Y, 800.0f),
		ESplineCoordinateSpace::World,
		true
	);

	// Spawn & setup projectile.

	Projectile = GetWorld()->SpawnActorDeferred<ASS_Projectile>(
		OwningPawnData.ProjectileClass,
		FTransform::Identity,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (Projectile)
	{
		UGameplayStatics::FinishSpawningActor(Projectile, FTransform::Identity);
	}

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

	if (TargetActor.IsValid())
	{
		SplineCT->SetLocationAtSplinePoint(2, TargetActor->GetActorLocation(), ESplineCoordinateSpace::World, true);
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
	Projectile->Destroy();
	this->Destroy();
}





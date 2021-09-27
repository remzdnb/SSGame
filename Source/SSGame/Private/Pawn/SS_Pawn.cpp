#include "Pawn/SS_Pawn.h"
#include "Pawn/SS_PawnAnimInstance.h"
#include "Weapon/SS_ProjectileSpline.h"
#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
#include "UI/SS_PawnOTMWidget.h"
//
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ASS_Pawn::ASS_Pawn()
{
	RootCT = CreateDefaultSubobject<USceneComponent>(FName("RootCT"));
	RootComponent = RootCT;

	MeshAxisCT = CreateDefaultSubobject<USceneComponent>(FName("MeshAxisCT"));
	MeshAxisCT->SetupAttachment(RootComponent);
	
	MeshCT = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MeshCT"));
	MeshCT->SetCollisionProfileName("NoCollision");
	MeshCT->SetupAttachment(MeshAxisCT);

	MeleeWeaponMeshCT = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeleeWeaponMeshCT"));
	MeleeWeaponMeshCT->SetCollisionProfileName("NoCollision");
	MeleeWeaponMeshCT->SetupAttachment(MeshCT, "Hand_R");

	RangedWeaponMeshCT = CreateDefaultSubobject<USkeletalMeshComponent>(FName("RangedWeaponMeshCT"));
	RangedWeaponMeshCT->SetCollisionProfileName("NoCollision");
	RangedWeaponMeshCT->SetupAttachment(MeshCT, "Hand_L");
	
	CollisionBoxCT = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBoxCT"));
	CollisionBoxCT->SetCollisionProfileName("PawnCollisionPreset");
	CollisionBoxCT->SetGenerateOverlapEvents(true);
	CollisionBoxCT->SetupAttachment(RootComponent);

	DetectionBoxCT = CreateDefaultSubobject<UBoxComponent>(FName("DetectionBoxCT"));
	DetectionBoxCT->SetCollisionProfileName("PawnDetectionPreset");
	DetectionBoxCT->SetGenerateOverlapEvents(true);
	DetectionBoxCT->SetupAttachment(RootComponent);
	
	OTMWidgetCT = CreateDefaultSubobject<UWidgetComponent>(FName("OTMWidgetCT"));
	OTMWidgetCT->SetupAttachment(MeshCT);

	//
	
	PrimaryActorTick.bCanEverTick = true;
}

void ASS_Pawn::Init(
	const FName& PawnDataRowName,
	const FSS_TileGroupData& SpawnTileGroup,
	ESS_Team NewTeam,
	bool bIsDemoPawn,
	bool bIsDemoPawnValidLocation)
{
	GInstance = Cast<USS_GameInstance>(GetGameInstance());
	AInstance = Cast<USS_PawnAnimInstance>(MeshCT->GetAnimInstance());

	for (TActorIterator<ASS_Grid> FoundGrid(GetWorld()); FoundGrid; ++FoundGrid)
	{
		Grid = *FoundGrid; // get from instance / gamemode ?
	}
	
	TileGroup = SpawnTileGroup;
	Team = NewTeam;
	PawnData = *GInstance->GetPawnDataFromRow(PawnDataRowName);

	Health = PawnData.MaxHealth;
	
	if (bIsDemoPawn)
		SetNewState(ESS_PawnState::Demo);
	else
		SetNewState(ESS_PawnState::Idling);

	// Mesh component.

	//MeshCT->SetSkeletalMesh(PawnData.MeshTemplate);

	if (PawnData.bIsMeshFacingXAxis == false)
	{
		MeshCT->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	
	if (Team == ESS_Team::South)
		DefaultRotation = FRotator(0.0f, 0.0f, 0.0f);
	else
		DefaultRotation = FRotator(0.0f, 180.0f, 0.0f);

	MeshAxisCT->SetWorldRotation(DefaultRotation);

	MeshRelativeLocation = FVector(
		(PawnData.Size - 1) * (TILESIZE / 2),
		(PawnData.Size - 1) * (TILESIZE / 2),
		0.0f
	); // ?
	
	MeshCT->SetRelativeLocation(FVector(
		(PawnData.Size - 1) * (TILESIZE / 2),
		(PawnData.Size - 1) * (TILESIZE / 2),
		0.0f
	));
	MeshCT->SetRelativeScale3D(PawnData.MeshScale);

	// Collision & detection components.
	
	if (bIsDemoPawn)
	{
		CollisionBoxCT->DestroyComponent();
		DetectionBoxCT->DestroyComponent();
		OTMWidgetCT->DestroyComponent();

		if (bIsDemoPawnValidLocation)
			MeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Valid);
		else
			MeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Invalid);
	}
	else
	{
		CollisionBoxCT->SetBoxExtent(FVector(
			PawnData.Size * TILESIZE / 2,
			PawnData.Size * TILESIZE / 2,
			TILESIZE / 2
		));
		CollisionBoxCT->SetRelativeLocation(FVector(
			(PawnData.Size - 1) * (TILESIZE / 2),
			(PawnData.Size - 1) * (TILESIZE / 2),
			TILESIZE / 2
		));

		DetectionBoxCT->SetBoxExtent(FVector(
			PawnData.AttackRange * TILESIZE / 2 - (TILESIZE / 2),
			PawnData.AttackRange * TILESIZE / 2 - (TILESIZE / 2),
			TILESIZE / 4
		));
		DetectionBoxCT->SetRelativeLocation(FVector(
			0.0f,
			0.0f,
			TILESIZE / 2
		));
	}

	// Projectile spline component.

	//ProjectileSplineCT->Add
	
	// Timelines

	MoveCurve = GInstance->GameSettings->LinearCurveFloat;
	if (MoveCurve)
	{
		FOnTimelineFloat MoveTimelineProgressCallback;
		FOnTimelineEventStatic MoveTimelineFinishedCallback;
		MoveTimelineProgressCallback.BindUFunction(this, FName("MoveTimelineProgress"));
		MoveTimelineFinishedCallback.BindUFunction(this, FName("MoveTimelineEnd"));
		MoveTimeline.AddInterpFloat(MoveCurve, MoveTimelineProgressCallback);
		MoveTimeline.SetTimelineFinishedFunc(MoveTimelineFinishedCallback);
		MoveTimeline.SetLooping(false);
		MoveTimeline.SetPlayRate(PawnData.MoveSpeed);
	}

	AttackCurve = GInstance->GameSettings->LinearCurveFloat;
	if (AttackCurve)
	{
		FOnTimelineFloat AttackTimelineProgressCallback;
		FOnTimelineEventStatic AttackTimelineFinishedCallback;
		AttackTimelineProgressCallback.BindUFunction(this, FName("AttackTimelineProgress"));
		AttackTimelineFinishedCallback.BindUFunction(this, FName("OnAttackTimelineEnd"));
		AttackTimeline.AddInterpFloat(AttackCurve, AttackTimelineProgressCallback);
		AttackTimeline.SetTimelineFinishedFunc(AttackTimelineFinishedCallback);
		AttackTimeline.SetLooping(false);
		AttackTimeline.SetPlayRate(PawnData.AttackSpeed);
	}
}

void ASS_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (OTMWidgetCT)
	{
		OTMWidget = Cast<USS_PawnOTMWidget>(OTMWidgetCT->GetWidget());
		if (OTMWidget)
			OTMWidget->Init(this);
	}
	
	DetectionBoxCT->OnComponentBeginOverlap.AddDynamic(this, &ASS_Pawn::OnDetectionBoxBeginOverlap);
	DetectionBoxCT->OnComponentEndOverlap.AddDynamic(this, &ASS_Pawn::OnDetectionBoxEndOverlap);
}

void ASS_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTimeline.TickTimeline(DeltaTime);
	AttackTimeline.TickTimeline(DeltaTime);

	if (TargetPawn.IsValid())
	{
		MeshAxisCT->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(),
			TargetPawn->GetActorLocation()
		));
	}
	else
	{
		MeshAxisCT->SetWorldRotation(DefaultRotation);
	}

	Debug(DeltaTime);
}

void ASS_Pawn::SetNewState(ESS_PawnState NewState)
{
	State = NewState;

	switch (State)
	{
	case ESS_PawnState::Moving:
		AInstance->PlayMoveStartAnimation();
		break;
	case ESS_PawnState::Attacking:
		AInstance->PlayAttackStartAnimation();
		break;
	default:
		AInstance->PlayIdleAnimation();
	}
}

#pragma region +++++ Movement ...

void ASS_Pawn::StartMoveToTileGroup()
{
	MoveStartLocation = GetActorLocation();
	MoveDirection = UKismetMathLibrary::FindLookAtRotation(
		MoveStartLocation,
		TileGroup.OriginTile->CenterLocation
	).Vector();
	MoveDistance = FVector::Dist(MoveStartLocation, TileGroup.OriginTile->CenterLocation);
	
	MoveTimeline.PlayFromStart();
}

void ASS_Pawn::MoveTimelineProgress(float Value)
{
	const FVector NewLocation = MoveStartLocation + MoveDirection * MoveDistance * Value;
	SetActorLocation(NewLocation);
}

void ASS_Pawn::MoveTimelineEnd()
{
	OnPawnActionCompletedEvent.Broadcast();
}

#pragma endregion 

#pragma region +++++ Combat ...

void ASS_Pawn::StartNewAttack(ASS_Pawn* NewTargetPawn)
{
	TargetPawn = NewTargetPawn;
	AttackTimeline.PlayFromStart();
}

void ASS_Pawn::OnAttackTimelineEnd()
{
	if (TargetPawn.IsValid())
	{
		if (PawnData.HasRangedAttack)
			ExecuteRangedAttack();
		else
			ExecuteMeleeAttack();

		TargetPawn.Reset();
	}
	
	OnPawnActionCompletedEvent.Broadcast();
}

void ASS_Pawn::ExecuteMeleeAttack()
{
	TargetPawn->ReceiveDamage(PawnData.AttackDamage);
}

void ASS_Pawn::ExecuteRangedAttack()
{
	const FTransform SpawnTransform = FTransform(
		MeshAxisCT->GetComponentRotation(),
		FVector(MeshCT->GetComponentLocation().X, MeshCT->GetComponentLocation().Y, 400.0f),
		FVector(1.0f)
	);
	
	ASS_ProjectileSpline* NewProjectileSpline = GetWorld()->SpawnActorDeferred<ASS_ProjectileSpline>(
		ASS_ProjectileSpline::StaticClass(),
		SpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (NewProjectileSpline)
	{
		NewProjectileSpline->Init(PawnData, TargetPawn.Get());
		UGameplayStatics::FinishSpawningActor(NewProjectileSpline, SpawnTransform);
	}
}

void ASS_Pawn::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.0f, PawnData.MaxHealth);
	if (Health <= 0)
	{
		Die();
	}

	UGameplayStatics::SpawnEmitterAttached(
		PawnData.HitParticle,
		MeshCT,
		"spine_03"
		);

	OnPawnHealthUpdatedEvent.Broadcast(Health, PawnData.MaxHealth);
}

void ASS_Pawn::Die()
{
	Grid->UnregisterPawnFromGrid(this);
	Destroy();
}

void ASS_Pawn::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	ASS_Pawn* DetectedPawn = Cast<ASS_Pawn>(OtherActor);
	if (DetectedPawn)
	{
		if (DetectedPawn->Team != Team && DetectedPawns.Contains(DetectedPawn) == false)
		{
			DetectedPawns.Add(DetectedPawn);
		}
	}
}

void ASS_Pawn::OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASS_Pawn* UndetectedPawn = Cast<ASS_Pawn>(OtherActor);
	if (UndetectedPawn)
	{
		if (DetectedPawns.Contains(UndetectedPawn) == true)
		{
			DetectedPawns.Remove(UndetectedPawn);
		}
	}
}

#pragma endregion

void ASS_Pawn::Debug(float DeltaTime)
{
	for (const auto& DetectedPawn : DetectedPawns)
	{
		const FVector StartVector = FVector(
			MeshCT->GetComponentLocation().X,
			MeshCT->GetComponentLocation().Y,
			TILESIZE / 2
			);

		const FVector EndVector = FVector(
			DetectedPawn->GetMesh()->GetComponentLocation().X,
			DetectedPawn->GetMesh()->GetComponentLocation().Y,
			TILESIZE / 2
		);
		
		UKismetSystemLibrary::DrawDebugArrow(
			GetWorld(),
			StartVector,
			EndVector,
			FVector::Dist(StartVector, EndVector),
			FColor::Magenta,
			DeltaTime - 0.1f,
			2
		);
	}
}

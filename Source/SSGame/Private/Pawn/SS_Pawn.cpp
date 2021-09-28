#include "Pawn/SS_Pawn.h"
#include "Pawn/SS_PawnAnimInstance.h"
#include "Weapon/SS_ProjectileSpline.h"
#include "Weapon/SS_Projectile.h"
#include "World/SS_Grid.h"
#include "World/SS_Tile.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
#include "UI/SS_PawnOTMWidget.h"
//
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
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
	MeshCT->SetCollisionProfileName("PawnMeshPreset");
	MeshCT->SetupAttachment(MeshAxisCT);

	MeleeWeaponMeshCT = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeleeWeaponMeshCT"));
	MeleeWeaponMeshCT->SetCollisionProfileName("NoCollision");
	MeleeWeaponMeshCT->SetupAttachment(MeshCT, "Hand_R");

	RangedWeaponMeshCT = CreateDefaultSubobject<USkeletalMeshComponent>(FName("RangedWeaponMeshCT"));
	RangedWeaponMeshCT->SetCollisionProfileName("NoCollision");
	RangedWeaponMeshCT->SetupAttachment(MeshCT, "Hand_L");

	CollisionCT = CreateDefaultSubobject<UBoxComponent>(FName("CollisionCT"));
	CollisionCT->SetCollisionProfileName("PawnCollisionPreset");
	CollisionCT->SetGenerateOverlapEvents(true);
	CollisionCT->SetupAttachment(RootComponent);

	MeleeDetectionCT = CreateDefaultSubobject<USphereComponent>(FName("MeleeDetectionCT"));
	MeleeDetectionCT->SetCollisionProfileName("PawnDetectionPreset");
	MeleeDetectionCT->SetGenerateOverlapEvents(true);
	MeleeDetectionCT->SetupAttachment(RootComponent);

	RangedDetectionCT = CreateDefaultSubobject<USphereComponent>(FName("RangedDetectionCT"));
	RangedDetectionCT->SetCollisionProfileName("PawnDetectionPreset");
	RangedDetectionCT->SetGenerateOverlapEvents(true);
	RangedDetectionCT->SetupAttachment(RootComponent);

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
		State = ESS_PawnState::Demo;
	else
		State = ESS_PawnState::Idle;

	// Mesh components

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

	MeshAxisCT->SetRelativeLocation(FVector(
		(PawnData.Size - 1) * (TILESIZE / 2),
		(PawnData.Size - 1) * (TILESIZE / 2),
		0.0f
	));
	MeshCT->SetRelativeScale3D(PawnData.MeshScale);

	if (bIsDemoPawn)
	{
		if (bIsDemoPawnValidLocation)
		{
			if (MeshCT)
				MeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Valid);
			if (MeleeWeaponMeshCT)
				MeleeWeaponMeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Valid);
			if (RangedWeaponMeshCT)
				RangedWeaponMeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Valid);
		}
		else
		{
			if (MeshCT)
				MeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Invalid);
			if (MeleeWeaponMeshCT)
				MeleeWeaponMeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Invalid);
			if (RangedWeaponMeshCT)
				RangedWeaponMeshCT->SetMaterial(0, GInstance->GameSettings->DemoPawnMaterial_Invalid);
		}
	}

	// Setup collision & detection components.

	if (bIsDemoPawn)
	{
		CollisionCT->DestroyComponent();
		MeleeDetectionCT->DestroyComponent();
		RangedDetectionCT->DestroyComponent();
		OTMWidgetCT->DestroyComponent(); // ?
	}
	else
	{
		CollisionCT->SetBoxExtent(FVector(
			PawnData.Size * TILESIZE / 2,
			PawnData.Size * TILESIZE / 2,
			TILESIZE / 2
		));
		CollisionCT->SetRelativeLocation(FVector(
			(PawnData.Size - 1) * (TILESIZE / 2),
			(PawnData.Size - 1) * (TILESIZE / 2),
			TILESIZE / 2
		));

		MeleeDetectionCT->SetSphereRadius((PawnData.MeleeAttackRange * TILESIZE / 2) + (TILESIZE / 2));
		MeleeDetectionCT->SetRelativeLocation(FVector(
			(PawnData.Size - 1) * (TILESIZE / 2),
			(PawnData.Size - 1) * (TILESIZE / 2),
			TILESIZE / 2
		));

		RangedDetectionCT->SetSphereRadius((PawnData.RangedAttackRange * TILESIZE / 2) + (TILESIZE / 2));
		RangedDetectionCT->SetRelativeLocation(FVector(
			(PawnData.Size - 1) * (TILESIZE / 2),
			(PawnData.Size - 1) * (TILESIZE / 2),
			TILESIZE / 2
		));

		if (GInstance->GameSettings->bDebugPawn)
		{
			CollisionCT->SetHiddenInGame(false);
			MeleeDetectionCT->SetHiddenInGame(false);
			RangedDetectionCT->SetHiddenInGame(false);
		}
	}

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

	MeleeDetectionCT->OnComponentBeginOverlap.AddDynamic(this, &ASS_Pawn::OnMeleeDetectionShereBeginOverlap);
	MeleeDetectionCT->OnComponentEndOverlap.AddDynamic(this, &ASS_Pawn::OnMeleeDetectionSphereEndOverlap);
	RangedDetectionCT->OnComponentBeginOverlap.AddDynamic(this, &ASS_Pawn::OnRangedDetectionShereBeginOverlap);
	RangedDetectionCT->OnComponentEndOverlap.AddDynamic(this, &ASS_Pawn::OnRangedDetectionSphereEndOverlap);
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

#pragma region +++++ Movement ...

void ASS_Pawn::StartNewMove(ESS_PawnMoveType NewMoveType)
{
	if (NewMoveType == ESS_PawnMoveType::Idle)
	{
		State = ESS_PawnState::Idle;
		AInstance->PlayIdleAnimation();
	}
	else
	{
		State = ESS_PawnState::Move;
		AInstance->PlayMoveAnimation();
	}
}

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

bool ASS_Pawn::StartNewAttack()
{
	if (MeleeDetectedPawns.Num() == 0 && RangedDetectedPawns.Num() == 0)
		return false;

	if (PawnData.PrimaryAttackType == ESS_PawnAttackType::Melee && PawnData.HasMeleeAttack)
	{
		if (MeleeDetectedPawns.Num() != 0)
		{
			TargetPawn = MeleeDetectedPawns[0];
			StartNewMeleeAttack();
			return true;
		}

		if (PawnData.HasRangedAttack && RangedDetectedPawns.Num() != 0)
		{
			TargetPawn = RangedDetectedPawns[0];
			StartNewRangedAttack();
			return true;
		}
	}

	if (PawnData.PrimaryAttackType == ESS_PawnAttackType::Ranged && PawnData.HasRangedAttack)
	{
		if (RangedDetectedPawns.Num() != 0)
		{
			TargetPawn = RangedDetectedPawns[0];
			StartNewRangedAttack();
			return true;
		}

		if (PawnData.HasMeleeAttack && MeleeDetectedPawns.Num() != 0)
		{
			TargetPawn = MeleeDetectedPawns[0];
			StartNewMeleeAttack();
			return true;
		}
	}

	return false;
}

void ASS_Pawn::StartNewMeleeAttack()
{
	State = ESS_PawnState::MeleeAttackStart;
	AttackTimeline.SetPlayRate(1 / PawnData.MeleeAttackStartDelay);
	AttackTimeline.PlayFromStart();
	AInstance->PlayMeleeAttackStartAnimation();
}

void ASS_Pawn::StartNewRangedAttack()
{
	LoadedProjectile = GetWorld()->SpawnActorDeferred<ASS_Projectile>(
		PawnData.ProjectileClass,
		FTransform::Identity,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (LoadedProjectile)
	{
		UGameplayStatics::FinishSpawningActor(LoadedProjectile, FTransform::Identity);

		if (RangedWeaponMeshCT->SkeletalMesh)
		{
			LoadedProjectile->AttachToComponent(
				RangedWeaponMeshCT,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				"ProjectileSocket"
			);
		}
		else
		{
			LoadedProjectile->AttachToComponent(
				MeshCT,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				"ProjectileSocket"
			);
		}

	}

	State = ESS_PawnState::RangedAttackStart;
	AttackTimeline.SetPlayRate(1 / PawnData.RangedAttackStartDelay);
	AttackTimeline.PlayFromStart();
	AInstance->PlayRangedAttackStartAnimation();
}

void ASS_Pawn::OnAttackTimelineEnd()
{
	// If target died or got destroyed, abort.
	if (TargetPawn.IsValid() == false || TargetPawn->State == ESS_PawnState::Dead)
	{
		OnPawnActionCompletedEvent.Broadcast();
		return;
	}
	
	if (State == ESS_PawnState::MeleeAttackStart)
	{
		ExecuteMeleeAttack();
		State = ESS_PawnState::MeleeAttackStop;
		AttackTimeline.SetPlayRate(1 / PawnData.MeleeAttackStopDelay);
		AttackTimeline.PlayFromStart();
		AInstance->PlayMeleeAttackStopAnimation();
	}
	else if (State == ESS_PawnState::RangedAttackStart)
	{
		ExecuteRangedAttack();
		State = ESS_PawnState::RangedAttackStop;
		AttackTimeline.SetPlayRate(1 / PawnData.RangedAttackStopDelay);
		AttackTimeline.PlayFromStart();
		AInstance->PlayRangedAttackStopAnimation();
	}
	else if (State == ESS_PawnState::MeleeAttackStop || State == ESS_PawnState::RangedAttackStop)
	{
		OnPawnActionCompletedEvent.Broadcast();
	}
}

void ASS_Pawn::ExecuteMeleeAttack()
{
	TargetPawn->ReceiveDamage(PawnData.MeleeAttackDamage);
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
		NewProjectileSpline->Init(PawnData, TargetPawn.Get(), LoadedProjectile);
		UGameplayStatics::FinishSpawningActor(NewProjectileSpline, SpawnTransform);
	}
}

void ASS_Pawn::ReceiveDamage(float Damage, class ASS_Projectile* Projectile)
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

	if (Projectile)
	{
		Projectile->AttachToComponent(MeshCT, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		AttachedProjectiles.Add(Projectile);
	}

	OnPawnHealthUpdatedEvent.Broadcast(Health, PawnData.MaxHealth);
}

void ASS_Pawn::Die()
{
	Grid->UnregisterPawnFromGrid(this);
	State = ESS_PawnState::Dead;

	//Controller->Destroy();

	// Destroy fired projectile ?

	for (const auto& Projectile : AttachedProjectiles)
	{
		Projectile->Destroy();
	}

	AInstance->PlayDeathAnimation();
	SetLifeSpan(5.0f);
	Destroy();
}

void ASS_Pawn::OnMeleeDetectionShereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepResult)
{
	ASS_Pawn* DetectedPawn = Cast<ASS_Pawn>(OtherActor);
	if (DetectedPawn)
	{
		if (DetectedPawn->Team != Team && MeleeDetectedPawns.Contains(DetectedPawn) == false)
		{
			MeleeDetectedPawns.Add(DetectedPawn);
		}
	}
}

void ASS_Pawn::OnMeleeDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASS_Pawn* UndetectedPawn = Cast<ASS_Pawn>(OtherActor);
	if (UndetectedPawn)
	{
		if (MeleeDetectedPawns.Contains(UndetectedPawn) == true)
		{
			MeleeDetectedPawns.Remove(UndetectedPawn);
		}
	}
}

void ASS_Pawn::OnRangedDetectionShereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                  const FHitResult& SweepResult)
{
	ASS_Pawn* DetectedPawn = Cast<ASS_Pawn>(OtherActor);
	if (DetectedPawn)
	{
		if (DetectedPawn->Team != Team && RangedDetectedPawns.Contains(DetectedPawn) == false)
		{
			RangedDetectedPawns.Add(DetectedPawn);
		}
	}
}

void ASS_Pawn::OnRangedDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASS_Pawn* UndetectedPawn = Cast<ASS_Pawn>(OtherActor);
	if (UndetectedPawn)
	{
		if (RangedDetectedPawns.Contains(UndetectedPawn) == true)
		{
			RangedDetectedPawns.Remove(UndetectedPawn);
		}
	}
}

#pragma endregion

void ASS_Pawn::Debug(float DeltaTime)
{
	/*for (const auto& DetectedPawn : DetectedPawns)
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
	}*/
}

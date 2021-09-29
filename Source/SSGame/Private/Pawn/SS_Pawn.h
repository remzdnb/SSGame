#pragma once

#include "SSGame.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "SS_Pawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPawnActionCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnStateUpdatedDelegate, ESS_PawnState, NewPawnState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPawnHealthUpdatedDelegate, float, NewHealth, float, NewMaxHealth);

UCLASS()
class ASS_Pawn : public APawn
{
	GENERATED_BODY()

public:
	
	ASS_Pawn();

	UFUNCTION()
	void Init(
		const FName& PawnDataRowName,
		const FSS_TileGroupData& InitialTileGroup,
		ESS_Team NewTeam = ESS_Team::Neutral,
		bool bIsDemoPawn = false,
		bool bIsDemoPawnValidLocation = false
	);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void Debug(float DeltaTime);

	///// ASS_Pawn
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	FOnPawnActionCompletedDelegate OnPawnActionCompletedEvent;
	FOnPawnStateUpdatedDelegate OnPawnStateUpdatedEvent;

	UPROPERTY() FSS_PawnData PawnData;
	UPROPERTY() ESS_Team Team;
	UPROPERTY() ESS_PawnState State;
	
	//

	UFUNCTION() class USkeletalMeshComponent* GetMesh() const { return MeshCT; }

private:
	
	// Scene Components

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MeshAxisCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* MeshCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeleeWeaponMeshCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* RangedWeaponMeshCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* MeleeDetectionCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RangedDetectionCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* GroundParticleCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OTMWidgetCT;
	
	// References

	class USS_GameInstance* GInstance;
	class USS_PawnAnimInstance* AInstance;
	class ASS_Grid* Grid;
	class USS_PawnOTMWidget* OTMWidget;

	///// Movement
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void StartNewMove(ESS_PawnMoveType NewMoveType);
	
	UFUNCTION()
	void StartMoveToTileGroup();

	//

	UPROPERTY()
	FSS_TileGroupData TileGroup;

private:

	UPROPERTY()
	FRotator DefaultRotation;
	
	UPROPERTY()
	UCurveFloat* MoveCurve;

	UPROPERTY()
	FTimeline MoveTimeline;

	UPROPERTY()
	float MoveTimelineProgressValue;

	UPROPERTY()
	FVector MoveStartLocation;

	UPROPERTY()
	FVector MoveDirection;

	UPROPERTY()
	float MoveDistance;

	//

	UFUNCTION()
	void MoveTimelineProgress(float Value);

	UFUNCTION()
	void MoveTimelineEnd();
	
	///// Combat
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	
	FOnPawnHealthUpdatedDelegate OnPawnHealthUpdatedEvent;

	TArray<TWeakObjectPtr<ASS_Pawn>> MeleeDetectedPawns;
	TArray<TWeakObjectPtr<ASS_Pawn>> RangedDetectedPawns;

	//

	UFUNCTION()
	bool StartNewAttack();

	UFUNCTION()
	void ReceiveDamage(float Damage, class ASS_Projectile* Projectile = nullptr);

private:

	TWeakObjectPtr<ASS_Pawn> TargetPawn;

	UPROPERTY()
	TArray<class ASS_Projectile*> AttachedProjectiles;
	
	UPROPERTY() UCurveFloat* AttackCurve;
	UPROPERTY() FTimeline AttackTimeline;
	UPROPERTY() float AttackTimelineProgressValue;
	UPROPERTY() class ASS_Projectile* LoadedProjectile;
	UPROPERTY() float Health;

	//

	UFUNCTION()
	void StartNewMeleeAttack();

	UFUNCTION()
	void StartNewRangedAttack();

	UFUNCTION()
	void OnAttackTimelineEnd();

	UFUNCTION()
	void ExecuteMeleeAttack();

	UFUNCTION()
	void ExecuteRangedAttack();
	
	UFUNCTION()
	void Die();
	
	UFUNCTION()
	void OnMeleeDetectionShereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                       const FHitResult& SweepResult);

	UFUNCTION()
	void OnMeleeDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRangedDetectionShereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                        const FHitResult& SweepResult);

	UFUNCTION()
	void OnRangedDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

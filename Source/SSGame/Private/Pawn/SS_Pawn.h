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

	// AActor
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	///// ASS_Pawn
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void SetNewState(ESS_PawnState NewState);
	
	//

	UFUNCTION() class USkeletalMeshComponent* GetMesh() const { return MeshCT; }
	
	//

	FOnPawnActionCompletedDelegate OnPawnActionCompletedEvent;
	FOnPawnStateUpdatedDelegate OnPawnStateUpdatedEvent;

	UPROPERTY()
	FSS_PawnData PawnData;

	UPROPERTY()
	ESS_Team Team;

	UPROPERTY()
	FVector MeshRelativeLocation;

	UPROPERTY()
	ESS_PawnState State;

private:

	UFUNCTION()
	void Debug(float DeltaTime);

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
	class UBoxComponent* CollisionBoxCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* DetectionBoxCT;

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
	void StartMoveToTileGroup();

	UFUNCTION()
	void MoveTimelineProgress(float Value);

	UFUNCTION()
	void MoveTimelineEnd();

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
	
	///// Combat
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void StartNewAttack(ASS_Pawn* NewTargetPawn);

	UFUNCTION()
	void ReceiveDamage(float Damage);
	
	//

	FOnPawnHealthUpdatedDelegate OnPawnHealthUpdatedEvent;
	
	TArray<TWeakObjectPtr<ASS_Pawn>> DetectedPawns;

private:

	UFUNCTION()
	void OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackTimelineEnd();

	UFUNCTION()
	void ExecuteMeleeAttack();

	UFUNCTION()
	void ExecuteRangedAttack();
	
	UFUNCTION()
	void Die();

	//
	
	TWeakObjectPtr<ASS_Pawn> TargetPawn;

	UPROPERTY()
	UCurveFloat* AttackCurve;

	UPROPERTY()
	FTimeline AttackTimeline;

	UPROPERTY()
	float AttackTimelineProgressValue;

	UPROPERTY()
	TArray<class ASS_Projectile*> ProjectileArray;

	UPROPERTY()
	float Health;
};

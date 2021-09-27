#pragma once

#include "SSGame.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SS_ProjectileSpline.generated.h"

UCLASS()
class ASS_ProjectileSpline : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASS_ProjectileSpline();
	
	UFUNCTION()
	void Init(const FSS_PawnData& NewOwningPawnData, AActor* NewTargetActor);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	// Scene components.

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* SplineCT;

	// Data

	UPROPERTY() FSS_PawnData OwningPawnData;

	// References
	
	class USS_GameInstance* GInstance;
	TWeakObjectPtr<AActor> TargetActor;

	///// Projectile
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION() void OnProjectileTimelineProgress(float NewProgressValue);
	UFUNCTION() void OnProjectileTimelineEnd();

	//

	UPROPERTY() class ASS_Projectile* Projectile;
	UPROPERTY() UCurveFloat* ProjectileTimelineCurve;
	UPROPERTY() FTimeline ProjectileTimeline;
	UPROPERTY() float SplineLength;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SS_Projectile.generated.h"

UCLASS()
class ASS_Projectile : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASS_Projectile();

	UFUNCTION()
	void Init(class ASS_ProjectileSpline* NewOwningProjectileSpline, class ASS_Pawn* NewTargetPawn);
	
	virtual void BeginPlay() override;

private:

	// Scene components

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshCT;

	// References

	TWeakObjectPtr<class ASS_ProjectileSpline> OwningProjectileSpline;
	TWeakObjectPtr<class ASS_Pawn> TargetPawn;
	
	//

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);
};

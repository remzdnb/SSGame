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
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshCT;

	//

	TWeakObjectPtr<AActor> TargetActor;
};

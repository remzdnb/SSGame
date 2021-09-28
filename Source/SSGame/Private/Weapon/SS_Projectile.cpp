#include "Weapon/SS_Projectile.h"
#include "Weapon/SS_ProjectileSpline.h"
#include "Pawn/SS_Pawn.h"
//
#include "Components/StaticMeshComponent.h"

ASS_Projectile::ASS_Projectile()
{
	RootCT = CreateDefaultSubobject<USceneComponent>(FName("RootCT"));
	RootComponent = RootCT;

	MeshCT = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshCT"));
	MeshCT->SetCollisionProfileName("ProjectilePreset");
	MeshCT->SetGenerateOverlapEvents(false);
	MeshCT->SetupAttachment(RootCT);

	//
	
	PrimaryActorTick.bCanEverTick = false;
}

void ASS_Projectile::Init(ASS_ProjectileSpline* NewOwningProjectileSpline, ASS_Pawn* NewTargetPawn)
{
	OwningProjectileSpline = NewOwningProjectileSpline;
	TargetPawn = NewTargetPawn; // ToDo : if target becomes invalid mid air ?
}

void ASS_Projectile::BeginPlay()
{
	Super::BeginPlay();

	//MeshCT->OnComponentBeginOverlap.AddDynamic(this, &ASS_Projectile::OnBeginOverlap);
}

void ASS_Projectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	if (OtherActor == TargetPawn.Get())
	{
		OwningProjectileSpline->Destroy();
		this->AttachToActor(TargetPawn.Get(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}



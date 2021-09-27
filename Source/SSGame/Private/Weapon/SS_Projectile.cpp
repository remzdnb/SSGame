#include "Weapon/SS_Projectile.h"
//
#include "Components/StaticMeshComponent.h"

ASS_Projectile::ASS_Projectile()
{
	RootCT = CreateDefaultSubobject<USceneComponent>(FName("RootCT"));
	RootComponent = RootCT;

	MeshCT = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshCT"));
	MeshCT->SetCollisionProfileName("NoCollision");
	MeshCT->SetGenerateOverlapEvents(false);
	MeshCT->SetupAttachment(RootCT);
	
	PrimaryActorTick.bCanEverTick = true;
}

void ASS_Projectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASS_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


#include "World/SS_Tile.h"
#include "Pawn/SS_Pawn.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
//
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ASS_Tile::ASS_Tile()
{
	RootSceneCT = CreateDefaultSubobject<USceneComponent>(FName("RootSceneCT"));
	RootComponent = RootSceneCT;

	MeshCT = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshCT"));
	MeshCT->SetCollisionProfileName("BlockAll");
	MeshCT->SetGenerateOverlapEvents(false);
	MeshCT->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	MeshCT->SetupAttachment(RootComponent);
	
	ParticleCT = CreateDefaultSubobject<UParticleSystemComponent>(FName("ParticleCT"));
	ParticleCT->SetupAttachment(RootComponent);

	//
	
	PrimaryActorTick.bCanEverTick = false;
}

void ASS_Tile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASS_Tile::BeginPlay()
{
	Super::BeginPlay();

	GInstance = Cast<USS_GameInstance>(GetGameInstance());

	CenterLocation = FVector(GetActorLocation().X + TILESIZE / 2, GetActorLocation().Y + TILESIZE / 2, 0.0f);
	
	if (TileData.Team == ESS_Team::Neutral)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->NeutralTileColor);
	if (TileData.Team == ESS_Team::South)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->EnemyTileColor);
	if (TileData.Team == ESS_Team::North)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->AllyTileColor);

	ParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->EmptyTileOpacity);
}

void ASS_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASS_Tile::RegisterPawnBPI_Implementation(ASS_Pawn* PawnToRegister)
{
	RegisteredPawn = PawnToRegister;

	if (RegisteredPawn->Team == ESS_Team::Neutral)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->NeutralTileColor);
	if (RegisteredPawn->Team == ESS_Team::South)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->EnemyTileColor);
	if (RegisteredPawn->Team == ESS_Team::North)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->AllyTileColor);
	
	ParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->OwnedTileOpacity);
}

void ASS_Tile::UnregisterPawnBPI_Implementation()
{
	RegisteredPawn.Reset();

	if (TileData.Team == ESS_Team::Neutral)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->NeutralTileColor);
	if (TileData.Team == ESS_Team::South)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->EnemyTileColor);
	if (TileData.Team == ESS_Team::North)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->AllyTileColor);

	ParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->EmptyTileOpacity);
}

void ASS_Tile::OnHoverStart(const FName& SelectedPawnRowName)
{
	if (RegisteredPawn == nullptr)
	{
		//ParticleCT->SetColorParameter("Color", GInstance->GameSettings->HoveredTileColor);
		//ParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->HoveredTileColor.A);
	}
}

void ASS_Tile::OnHoverStop()
{
	if (RegisteredPawn == nullptr)
	{
		//ParticleCT->SetColorParameter("Color", GInstance->GameSettings->EmptyTileColor);
		//ParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->EmptyTileColor.A);
	}
}

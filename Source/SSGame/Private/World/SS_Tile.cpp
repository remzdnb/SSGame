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
	
	SouthBorderParticleCT = CreateDefaultSubobject<UParticleSystemComponent>(FName("SouthBorderParticleCT"));
	SouthBorderParticleCT->SetupAttachment(RootComponent);

	NorthBorderParticleCT = CreateDefaultSubobject<UParticleSystemComponent>(FName("NorthBorderParticleCT"));
	NorthBorderParticleCT->SetupAttachment(RootComponent);

	WestBorderParticleCT = CreateDefaultSubobject<UParticleSystemComponent>(FName("WestBorderParticleCT"));
	WestBorderParticleCT->SetupAttachment(RootComponent);

	EastBorderParticleCT = CreateDefaultSubobject<UParticleSystemComponent>(FName("EastBorderParticleCT"));
	EastBorderParticleCT->SetupAttachment(RootComponent);

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

	FLinearColor TileParticleColor;
	if (TileData.Team == ESS_Team::Neutral)
		TileParticleColor = GInstance->GameSettings->NeutralTileParticleColor;
	if (TileData.Team == ESS_Team::South)
		TileParticleColor = GInstance->GameSettings->EnemyTileParticleColor;
	if (TileData.Team == ESS_Team::North)
		TileParticleColor = GInstance->GameSettings->AllyTileParticleColor;
	

	SouthBorderParticleCT->SetColorParameter("Color", TileParticleColor);
	NorthBorderParticleCT->SetColorParameter("Color", TileParticleColor);
	WestBorderParticleCT->SetColorParameter("Color", TileParticleColor);
	EastBorderParticleCT->SetColorParameter("Color", TileParticleColor);
	SouthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
	NorthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
	WestBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
	EastBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
}

void ASS_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASS_Tile::RegisterPawnBPI_Implementation(ASS_Pawn* PawnToRegister)
{
	RegisteredPawn = PawnToRegister;

	/*if (RegisteredPawn->Team == ESS_Team::Neutral)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->NeutralTileColor);
	if (RegisteredPawn->Team == ESS_Team::South)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->EnemyTileColor);
	if (RegisteredPawn->Team == ESS_Team::North)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->AllyTileColor);
	
	ParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->OwnedTileOpacity);*/
}

void ASS_Tile::UnregisterPawnBPI_Implementation()
{
	RegisteredPawn.Reset();

	/*if (TileData.Team == ESS_Team::Neutral)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->NeutralTileColor);
	if (TileData.Team == ESS_Team::South)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->EnemyTileColor);
	if (TileData.Team == ESS_Team::North)
		ParticleCT->SetColorParameter("Color", GInstance->GameSettings->AllyTileColor);

	ParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->EmptyTileOpacity);*/
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

void ASS_Tile::HighlightColumnParticles(bool bEnable, bool bIsStrongHighlight)
{
	if (bEnable)
	{
		if (bIsStrongHighlight)
		{
			WestBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->StrongHighlightTileParticleOpacity);
			EastBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->StrongHighlightTileParticleOpacity);
		}
		else
		{
			WestBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->LightHighlightTileParticleOpacity);
			EastBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->LightHighlightTileParticleOpacity);
		}
	}
	else
	{
		WestBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
		EastBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
	}
}

void ASS_Tile::HighlightLineParticles(bool bEnable, bool bIsStrongHighlight)
{
	if (bEnable)
	{
		if (bIsStrongHighlight)
		{
			SouthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->StrongHighlightTileParticleOpacity);
			NorthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->StrongHighlightTileParticleOpacity);
		}
		else
		{
			SouthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->LightHighlightTileParticleOpacity);
			NorthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->LightHighlightTileParticleOpacity);
		}
	}
	else
	{
		SouthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
		NorthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->DefaultTileParticleOpacity);
	}
}

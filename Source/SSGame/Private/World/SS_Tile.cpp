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
		TileParticleColor = GInstance->GameSettings->TileParticleColor_01;
	if (TileData.Team == ESS_Team::South)
		TileParticleColor = GInstance->GameSettings->EnemyTileParticleColor;
	if (TileData.Team == ESS_Team::North)
		TileParticleColor = GInstance->GameSettings->AllyTileParticleColor;
	

	//SouthBorderParticleCT->SetColorParameter("Color", FColor(100.0f,100.0f,100.0f));
	/*NorthBorderParticleCT->SetColorParameter("Color", TileParticleColor);
	WestBorderParticleCT->SetColorParameter("Color", TileParticleColor);
	EastBorderParticleCT->SetColorParameter("Color", TileParticleColor);
	SouthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->TileParticleColor_01.A);
	NorthBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->TileParticleColor_01.A);
	WestBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->TileParticleColor_01.A);
	EastBorderParticleCT->SetFloatParameter("Opacity", GInstance->GameSettings->TileParticleColor_01.A);*/
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
			//WestBorderParticleCT->SetColorParameter("Color", FLinearColor(0.0f,1.0f,0.0f));
			WestBorderParticleCT->SetFloatParameter("Opacity", 0.1f);
			//EastBorderParticleCT->SetColorParameter("Color", FLinearColor(0.0f,0.0f,1.0f));
			EastBorderParticleCT->SetFloatParameter("Opacity", 0.1f);
		}
		else
		{
			//WestBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_02);
			WestBorderParticleCT->SetFloatParameter("Opacity", 0.01f);
			//EastBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_02);
			EastBorderParticleCT->SetFloatParameter("Opacity", 0.01f);
		}
	}
	else
	{
		//WestBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_01);
		WestBorderParticleCT->SetFloatParameter("Opacity", 0.0005f);
		//EastBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_01);
		EastBorderParticleCT->SetFloatParameter("Opacity", 0.0005f);
	}
}

void ASS_Tile::HighlightLineParticles(bool bEnable, bool bIsStrongHighlight)
{
	if (bEnable)
	{
		if (bIsStrongHighlight)
		{
			//SouthBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_03);
			SouthBorderParticleCT->SetFloatParameter("Opacity", 0.1f);
			//NorthBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_03);
			NorthBorderParticleCT->SetFloatParameter("Opacity", 0.1f);
		}
		else
		{
			//SouthBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_02);
			SouthBorderParticleCT->SetFloatParameter("Opacity", 0.01f);
			//NorthBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_02);
			NorthBorderParticleCT->SetFloatParameter("Opacity", 0.01f);
		}
	}
	else
	{
		//SouthBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_01);
		SouthBorderParticleCT->SetFloatParameter("Opacity", 0.0005f);
		//NorthBorderParticleCT->SetColorParameter("Color", GInstance->GameSettings->TileParticleColor_01);
		NorthBorderParticleCT->SetFloatParameter("Opacity", 0.0005f);
	}
}

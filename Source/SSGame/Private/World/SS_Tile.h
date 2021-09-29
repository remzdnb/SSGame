#pragma once

#include "SSGame.h"
#include "GameFramework/Actor.h"
#include "Pawn/SS_Pawn.h"
#include "SS_Tile.generated.h"

UCLASS()
class ASS_Tile : public AActor
{
	GENERATED_BODY()

public:

	ASS_Tile();

	// Actor
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// Interaction

	UFUNCTION()
	void OnHoverStart(const FName& SelectedPawnRowName);

	UFUNCTION()
	void OnHoverStop();

	// Data

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FSS_TileData TileData;

private:

	// Scene Components
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* SouthBorderParticleCT;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* NorthBorderParticleCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* WestBorderParticleCT;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* EastBorderParticleCT;
	
	// References

public:

	UPROPERTY()
	FVector CenterLocation;

private:

	class USS_GameInstance* GInstance;
	
	///// Pawn
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION(BlueprintNativeEvent)
	void RegisterPawnBPI(ASS_Pawn* PawnToRegister);

	UFUNCTION(BlueprintNativeEvent)
	void UnregisterPawnBPI();

	//

	FORCEINLINE UFUNCTION() TWeakObjectPtr<ASS_Pawn> GetRegisteredPawn() const { return RegisteredPawn; }
	
private:

	TWeakObjectPtr<ASS_Pawn> RegisteredPawn;

	///// Rendering
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	
	UFUNCTION()
	void HighlightColumnParticles(bool bEnable, bool bIsStrongHighlight = false);

	UFUNCTION()
	void HighlightLineParticles(bool bEnable, bool bIsStrongHighlight = false);
};
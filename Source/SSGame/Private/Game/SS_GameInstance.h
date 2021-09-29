///// SS_GameInstance.h - RemzDNB
/////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SSGame.h"
//
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SS_GameInstance.generated.h"

class UDataTable;
class USS_GameSettings;

UCLASS()
class USS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void StartGameInstance() override;
	virtual void OnStart() override;
	virtual void Init() override;

	//

	class ASS_Grid* Grid;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///// Data

public:

	const FSS_PawnData* const GetPawnDataFromRow(const FName& RowName) const;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* PawnDT;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USS_GameSettings* GameSettings;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///// Console commands

private:
	
	UFUNCTION(Exec)
	void ToggleDebug(bool bEnable);
	
};

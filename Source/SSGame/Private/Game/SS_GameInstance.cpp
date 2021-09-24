#include "Game/SS_GameInstance.h"
//
#include "EngineUtils.h"

void USS_GameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

void USS_GameInstance::OnStart()
{
	Super::OnStart();
}

void USS_GameInstance::Init()
{
	Super::Init();
}

const FSS_PawnData* const USS_GameInstance::GetPawnDataFromRow(const FName& RowName) const
{
	if (PawnDT == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USS_GameInstance::GetVehicleDataFromRow : Missing PawnDT."));
		return nullptr;
	}

	const FString ContextString;
	const FSS_PawnData* const VehicleData = PawnDT->FindRow<FSS_PawnData>(RowName, ContextString);
	
	return VehicleData;
}

void USS_GameInstance::ToggleDebug(bool bEnable)
{
	//DebugSettings.bIsDebugEnabled_WeaponModule = bEnable;
}

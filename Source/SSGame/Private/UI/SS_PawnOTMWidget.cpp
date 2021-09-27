#include "UI/SS_PawnOTMWidget.h"
#include "Pawn/SS_Pawn.h"
//
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "World/SS_Tile.h"

void USS_PawnOTMWidget::Init(ASS_Pawn* NewPawnOwner)
{
	PawnOwner = NewPawnOwner;

	PawnOwner->OnPawnHealthUpdatedEvent.AddUniqueDynamic(this, &USS_PawnOTMWidget::OnHealthUpdated);
}

void USS_PawnOTMWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USS_PawnOTMWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PawnOwner)
	{
		StateText->SetText(FText::FromString(GetEnumAsString("ESS_PawnState", PawnOwner->State)));

		if (PawnOwner->TileGroup.OriginTile)
		{
			TileText->SetText(FText::FromString(
				"PositionX : " +
				FString::FromInt(PawnOwner->TileGroup.OriginTile->TileData.PositionX) +
				" // PositionY : " +
				FString::FromInt(PawnOwner->TileGroup.OriginTile->TileData.PositionY)
			));
		}
	}
}

void USS_PawnOTMWidget::OnHealthUpdated(float NewHealth, float NewMaxHealth)
{
	HealthProgressBar->SetPercent(NewHealth / NewMaxHealth);
}

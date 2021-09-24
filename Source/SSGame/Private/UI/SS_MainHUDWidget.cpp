#include "UI/SS_MainHUDWidget.h"
#include "UI/SS_CharacterSelectionItemWidget.h"
#include "Player/SS_PlayerController.h"
#include "Game/SS_GameInstance.h"
#include "Game/SS_GameSettings.h"
//
#include "Components/PanelWidget.h"

void USS_MainHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GInstance = Cast<USS_GameInstance>(GetGameInstance());
	PController = Cast<ASS_PlayerController>(GetOwningPlayer());

	PController->OnNewSelectedPawnEvent.AddDynamic(this, &USS_MainHUDWidget::UpdateCharacterSelectionPanel);
	
	//

	UpdateCharacterSelectionPanel();
}

void USS_MainHUDWidget::UpdateCharacterSelectionPanel()
{
	CharacterSelectionPanel->ClearChildren();
	
	for (const auto& CharacterRowName : GInstance->PawnDT->GetRowNames())
	{
		USS_CharacterSelectionItemWidget* const CharacterSelectionItemWidget = CreateWidget<USS_CharacterSelectionItemWidget>(
			GetWorld(),
			GInstance->GameSettings->CharacterSelectionItem_WBP
		);
		if (CharacterSelectionItemWidget)
		{
			CharacterSelectionItemWidget->Update(CharacterRowName);
			CharacterSelectionPanel->AddChild(CharacterSelectionItemWidget);
		}
	}
}

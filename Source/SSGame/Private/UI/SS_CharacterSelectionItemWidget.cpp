#include "UI/SS_CharacterSelectionItemWidget.h"
#include "Player/SS_PlayerController.h"
//
#include "Components/TextBlock.h"

void USS_CharacterSelectionItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PController = Cast<ASS_PlayerController>(GetOwningPlayer());
}

FReply USS_CharacterSelectionItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	PController->SelectCharacterToSpawn(CharacterRowName);
	
	return FReply::Handled();
}

void USS_CharacterSelectionItemWidget::Update(const FName& NewCharacterRowName)
{
	CharacterRowName = NewCharacterRowName;

	CharacterRowNameText->SetText(FText::FromName(CharacterRowName));

	if (PController->GetCharacterToSpawnRowName() == NewCharacterRowName)
		SelectBPI();
}

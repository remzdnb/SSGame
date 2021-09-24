#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SS_CharacterSelectionItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterSelectionItemPressedDelegate, const FName&, CharacterRowName);

UCLASS()
class USS_CharacterSelectionItemWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

/////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION()
	void Update(const FName& NewCharacterRowName);

	UFUNCTION(BlueprintImplementableEvent)
	void SelectBPI();

	//

	FCharacterSelectionItemPressedDelegate OnSlotSelectionItemPressed;

private:

	class ASS_PlayerController* PController;
	
	//
	
	UPROPERTY()
	FName CharacterRowName;

	//

	UPROPERTY(meta = (BindWidget)) class UTextBlock* CharacterRowNameText;
};

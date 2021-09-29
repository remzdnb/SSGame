#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SS_MainHUDWidget.generated.h"

UCLASS()
class USS_MainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	//

	UFUNCTION(BlueprintImplementableEvent)
	void TogglePawnSelectionPanelBPI(bool bNewIsVisible);

private:

	UFUNCTION()
	void UpdateCharacterSelectionPanel();

	//

	class USS_GameInstance* GInstance;
	class ASS_PlayerController* PController;

	UPROPERTY(meta = (BindWidget)) UPanelWidget* CharacterSelectionPanel;
};

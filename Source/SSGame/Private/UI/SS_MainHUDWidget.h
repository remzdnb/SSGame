#pragma once

#include "SSGame.h"
#include "Blueprint/UserWidget.h"
#include "SS_MainHUDWidget.generated.h"

class USS_GameInstance;
class ASS_GameState;
class ASS_PlayerController;
class UPanelWidget;
class UTextBlock;

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
	
	USS_GameInstance* GInstance;
	ASS_GameState* GState;
	ASS_PlayerController* PController;

	UPROPERTY(meta = (BindWidget)) UPanelWidget* CharacterSelectionPanel;
	UPROPERTY(meta = (BindWidget)) UTextBlock* GamePhaseText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* GamePhaseCooldownText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* GoldAmountText;

	//

	UFUNCTION()
	void UpdateCharacterSelectionPanel();

	UFUNCTION()
	void OnGamePhaseUpdated(ESS_GamePhase NewGamePhase);
};

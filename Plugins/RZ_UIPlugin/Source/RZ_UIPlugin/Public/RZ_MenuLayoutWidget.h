#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RZ_MenuLayoutWidget.generated.h"

UCLASS()
class RZ_UIPLUGIN_API URZ_MenuLayoutWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	//

	UFUNCTION()
	void Init(class ARZ_UIManager* NewUIManager);

	UFUNCTION() // Load UserWidget into the widget switcher and create associated top bar tab.
	class UUserWidget* CreateMenuWidget(const FName& TabName, TSubclassOf<UUserWidget> WidgetClass, bool bShouldCreateTab = true, class URZ_ButtonWidget* TabToAssign = nullptr);

	UFUNCTION()
	void SetActiveWidgetByIndex(uint8 Index);

	UFUNCTION()
	void SetActiveWidgetByRef(UUserWidget* WidgetRef);

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewActiveWidgetBPI();

	//

	UFUNCTION() FORCEINLINE UUserWidget* GetLoadedWidgetByName(const FName& WidgetName) const { return LoadedWidgets.FindRef(WidgetName); }

private:

	class ARZ_UIManager* UIManager;
	
	UPROPERTY()
	TArray<class URZ_ButtonWidget*> TabWidgets;

	UPROPERTY()
	TMap<FName, UUserWidget*> LoadedWidgets;

	UPROPERTY()
	uint8 SwitcherIndex;

	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ButtonWBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ExitWBP;

	//

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* TopBarPanel;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	class URZ_ButtonWidget* ExitTab;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RZ_UIManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewMenuWidgetSelectedDelegate, const FName&, WidgetName, UUserWidget*, SelectedWidget);

UCLASS()
class RZ_UIPLUGIN_API ARZ_UIManager : public AHUD
{
	GENERATED_BODY()

public:

	ARZ_UIManager();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	//

	UFUNCTION() void ToggleMenu(bool bNewIsOpen);
	UFUNCTION() void ToggleHUD(bool bNewIsOpen);
	UFUNCTION() void AddHUDWidget(UUserWidget* NewWidget, bool bSaveReference = true);
	UFUNCTION() void BroadcastMenuWidgetSelected(const FName& WidgetName, UUserWidget* WidgetRef);

	//

	UFUNCTION() FORCEINLINE class URZ_MenuLayoutWidget* GetMenuLayoutWidget() const { return MenuLayoutWidget; }
	UFUNCTION() FORCEINLINE bool IsMenuOpen() const { return bIsMenuOpen; }
	UFUNCTION() FORCEINLINE bool IsHUDOpen() const { return bIsHUDOpen; }

	//

	FNewMenuWidgetSelectedDelegate OnNewMenuWidgetSelected;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class URZ_MenuLayoutWidget> MenuLayoutWBP;
	
	UPROPERTY()
	class URZ_MenuLayoutWidget* MenuLayoutWidget;

	UPROPERTY()
	TArray<UUserWidget*> HUDWidgets;
	
	bool bIsMenuOpen;
	bool bIsHUDOpen;
};
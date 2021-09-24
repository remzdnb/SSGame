#include "RZ_UIManager.h"
#include "RZ_MenuLayoutWidget.h"
//
#include "Components/PanelWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "EngineUtils.h"

ARZ_UIManager::ARZ_UIManager() :
	bIsMenuOpen(false),
	bIsHUDOpen(false)
{
}

void ARZ_UIManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld()->IsGameWorld() == false)
		return;

	MenuLayoutWidget = CreateWidget<URZ_MenuLayoutWidget>(GetOwningPlayerController(), MenuLayoutWBP);
	MenuLayoutWidget->Init(this);
}

void ARZ_UIManager::BeginPlay()
{
	Super::BeginPlay();
}

void ARZ_UIManager::AddHUDWidget(UUserWidget* NewWidget, bool bSaveReference)
{
	if (bSaveReference)
	{
		HUDWidgets.Add(NewWidget);
	}
	
	if (IsHUDOpen())
	{
		NewWidget->AddToViewport();
	}
	
	/*if (BattleHUDWidget == nullptr)
	return;
	
	BattleHUDWidget->GetMainCanvas()->AddChild(NewWidget);

	UCanvasPanelSlot* const CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(NewWidget);
	CanvasPanelSlot->bAutoSize = true;*/ // ?? Refresh
}

void ARZ_UIManager::BroadcastMenuWidgetSelected(const FName& WidgetName, UUserWidget* WidgetRef)
{
	OnNewMenuWidgetSelected.Broadcast(WidgetName, WidgetRef);
}

void ARZ_UIManager::ToggleMenu(bool bNewIsOpen)
{
	if (MenuLayoutWidget == nullptr)
		return;

	if (bNewIsOpen == true)
	{
		if (MenuLayoutWidget->IsInViewport() == false)
			MenuLayoutWidget->AddToViewport();
		
		bIsMenuOpen = true;
	}
	else
	{
		if (MenuLayoutWidget->IsInViewport() == true)
			MenuLayoutWidget->RemoveFromParent();

		bIsMenuOpen = false;
	}
}

void ARZ_UIManager::ToggleHUD(bool bNewIsOpen)
{
	if (bNewIsOpen == true && IsHUDOpen() == false)
	{
		for (const auto& HUDWidget : HUDWidgets)
		{
			HUDWidget->AddToViewport();
			bIsHUDOpen = true;
		}
	}
	
	if (bNewIsOpen == false && IsHUDOpen() == true)
	{
		for (const auto& HUDWidget : HUDWidgets)
		{
			HUDWidget->RemoveFromParent();
			bIsHUDOpen = false;
		}
	}
}
#pragma once

#include "SSGame.h"
#include "Blueprint/UserWidget.h"
#include "SS_PawnOTMWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class USS_PawnOTMWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void Init(class ASS_Pawn* NewPawnOwner);

protected:
	
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	UFUNCTION()
	void OnHealthUpdated(float NewHealth, float NewMaxHealth);
	
	ASS_Pawn* PawnOwner;
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* StateText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TileText;
	UPROPERTY(meta = (BindWidget)) UProgressBar* HealthProgressBar;
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RZ_ComboBoxWidget.generated.h"

UCLASS()
class RZ_UIPLUGIN_API URZ_ComboBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* CBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image;

	//

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString TitleString;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTexture2D* ImageTexture;
};

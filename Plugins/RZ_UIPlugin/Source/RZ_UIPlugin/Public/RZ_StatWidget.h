#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RZ_StatWidget.generated.h"


UCLASS()
class RZ_UIPLUGIN_API URZ_StatWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void Init(float NewStatValue);
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString StatName;

	//

	UPROPERTY(meta = (BindWidget)) class UTextBlock* StatNameText;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* StatValueText;
	UPROPERTY(meta = (BindWidget)) class URZ_ProgressBarWidget* StatProgressBar;
};

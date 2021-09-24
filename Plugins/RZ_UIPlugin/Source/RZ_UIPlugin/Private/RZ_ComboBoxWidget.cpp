#include "RZ_ComboBoxWidget.h"
//
#include "Components/TextBlock.h"
#include "Components/Image.h"

void URZ_ComboBoxWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (TitleString.IsEmpty() == false)
		TitleTextBlock->SetText(FText::FromString(TitleString));

	if (ImageTexture)
		Image->SetBrushFromTexture(ImageTexture, false);
}

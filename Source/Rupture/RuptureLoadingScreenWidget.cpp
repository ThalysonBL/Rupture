#include "RuptureLoadingScreenWidget.h"
#include "Components/TextBlock.h"

void URuptureLoadingScreenWidget::SetStatusText(const FText& InText)
{
	if (txt_Status)
	{
		txt_Status->SetText(InText);
	}
}

#include "RuptureEnemyhealthWidget.h"
#include "Components/ProgressBar.h"

void URuptureEnemyhealthWidget::UpdateHealth(const float CurrentHealth , const float MaxHealth)
{
	if (pb_Health && MaxHealth > 0.f)
	{
		pb_Health->SetPercent(CurrentHealth / MaxHealth);
	}
}

void URuptureEnemyhealthWidget::ShowBar()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void URuptureEnemyhealthWidget::HideBar()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
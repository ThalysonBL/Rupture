#include "RuptureVictoryScreenWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void URuptureVictoryScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (btn_BackToMenu)
	{
		btn_BackToMenu->OnClicked.AddDynamic(this, &URuptureVictoryScreenWidget::OnBackToMenuClicked);
	}
}

void URuptureVictoryScreenWidget::ShowVictory()
{
	SetVisibility(ESlateVisibility::Visible);

	if (txt_Title)
	{
		txt_Title->SetText(FText::FromString(TEXT("Parabéns!")));
	}

	if (txt_Message)
	{
		txt_Message->SetText(FText::FromString(TEXT("Você completou os 10 rounds!")));
	}

	if (AutoReturnDelay > 0.f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			AutoReturnTimerHandle,
			this,
			&URuptureVictoryScreenWidget::ReturnToMainMenu,
			AutoReturnDelay,
			false
		);
	}

	UE_LOG(LogTemp, Warning, TEXT("Victory: tela de parabéns exibida."));
}

void URuptureVictoryScreenWidget::OnBackToMenuClicked()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoReturnTimerHandle);
	}
	ReturnToMainMenu();
}

void URuptureVictoryScreenWidget::ReturnToMainMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Victory: voltando ao menu -> %s"), *MainMenuLevelName.ToString());
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}

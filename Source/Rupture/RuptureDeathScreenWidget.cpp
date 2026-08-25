
#include "RuptureDeathScreenWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RuptureWaveManager.h"
#include "RupturePlayerCharacter.h"
#include "RuptureHUD.h"

void URuptureDeathScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (btn_RestartRound)
    {
        btn_RestartRound->OnClicked.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartRoundClicked);
    }
    if (btn_RestartAll)
    {
        btn_RestartAll->OnClicked.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartAllClicked);
    }
}

void URuptureDeathScreenWidget::SetRoundInfo(int32 RoundNumber)
{
    if (txt_RoundInfo)
	{
		txt_RoundInfo->SetText(
			FText::FromString(FString::Printf(TEXT("Você morreu no Round %d"), RoundNumber))
		);
	}
}

void URuptureDeathScreenWidget::OnRestartRoundClicked()
{
    HandleRestart(false);
}

void URuptureDeathScreenWidget::OnRestartAllClicked()
{
    HandleRestart(true);
}

ARuptureWaveManager* URuptureDeathScreenWidget::FindWaveManager() const
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARuptureWaveManager::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        return Cast<ARuptureWaveManager>(FoundActors[0]);
    }
    return nullptr;
}

void URuptureDeathScreenWidget::HandleRestart(bool bRestartAll)
{
    ARuptureWaveManager* WaveManager = FindWaveManager();
    if (WaveManager)
    {
        if (bRestartAll)
        {
            WaveManager->RestartAllRounds();
        }
        else
        {
            WaveManager->RestartCurrentRound();
        }
    }
	if (ARupturePlayerCharacter* Player = Cast<ARupturePlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->Revive();
	}
    if (APlayerController* PC = GetOwningPlayer())
	{
		if (ARuptureHUD* HUD = Cast<ARuptureHUD>(PC->GetHUD()))
		{
			HUD->HideDeathScreen();
		}
	}
}
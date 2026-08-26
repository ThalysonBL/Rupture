#include "RuptureHUD.h"
#include "GameFramework/PlayerController.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RuptureDeathScreenWidget.h"
#include "RupturePlayerCharacter.h"
#include "RupturePlayerHUDWidget.h"
#include "RuptureVictoryScreenWidget.h"
#include "RuptureWaveManager.h"
#include "RuptureWeaponBase.h"

void ARuptureHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<URupturePlayerHUDWidget>(GetWorld(), PlayerHUDClass);

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();

			ARupturePlayerCharacter* PlayerChar = Cast<ARupturePlayerCharacter>(
				UGameplayStatics::GetPlayerCharacter(this, 0));
			if (PlayerChar)
			{
				UHealthComponent* HealthComp = PlayerChar->FindComponentByClass<UHealthComponent>();
				if (HealthComp)
				{
					HealthComp->OnHealthChanged.AddDynamic(this, &ARuptureHUD::OnPlayerHealthChanged);
					HealthComp->OnDeath.AddDynamic(this, &ARuptureHUD::OnPlayerDied);
					PlayerHUDWidget->UpdateHealth(HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
				}

				PlayerChar->OnWeaponEquipped.AddDynamic(this, &ARuptureHUD::OnWeaponEquippedCallback);
			}
		}
	}

	if (DeathScreenClass)
	{
		DeathScreenWidget = CreateWidget<URuptureDeathScreenWidget>(GetWorld(), DeathScreenClass);
	}

	if (VictoryScreenClass)
	{
		VictoryScreenWidget = CreateWidget<URuptureVictoryScreenWidget>(GetWorld(), VictoryScreenClass);
	}

	BindWaveManagerEvents();
}

void ARuptureHUD::BindWaveManagerEvents()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARuptureWaveManager::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		if (ARuptureWaveManager* WM = Cast<ARuptureWaveManager>(Found[0]))
		{
			WM->OnAllRoundsCompleted.AddDynamic(this, &ARuptureHUD::OnAllRoundsCompleted);
			UE_LOG(LogTemp, Warning, TEXT("HUD: inscrito em OnAllRoundsCompleted."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: WaveManager não encontrado no BeginPlay."));
	}
}

void ARuptureHUD::OnPlayerHealthChanged(float NewHealth, float MaxHealth)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->UpdateHealth(NewHealth, MaxHealth);
	}
}

void ARuptureHUD::OnWeaponEquippedCallback(ARuptureWeaponBase* NewWeapon)
{
	if (NewWeapon)
	{
		NewWeapon->OnAmmoChanged.AddDynamic(this, &ARuptureHUD::OnAmmoChangedCallback);

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->UpdateAmmo(NewWeapon->GetCurrentAmmo(), NewWeapon->GetReserveAmmo());
		}
	}
}

void ARuptureHUD::OnAmmoChangedCallback(int32 CurrentAmmo, int32 ReserveAmmo)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->UpdateAmmo(CurrentAmmo, ReserveAmmo);
	}
}

void ARuptureHUD::OnPlayerDied()
{
	UE_LOG(LogTemp, Warning, TEXT("HUD: player morreu — mostrando death screen."));
	ShowDeathScreen();
}

void ARuptureHUD::ShowDeathScreen()
{
	if (!DeathScreenWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("HUD: DeathScreenWidget é null. Sete DeathScreenClass no BP_RuptureHUD."));
		return;
	}

	if (!DeathScreenWidget->IsInViewport())
	{
		DeathScreenWidget->AddToViewport(10);
	}

	DeathScreenWidget->SetVisibility(ESlateVisibility::Visible);

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARuptureWaveManager::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		if (ARuptureWaveManager* WM = Cast<ARuptureWaveManager>(Found[0]))
		{
			DeathScreenWidget->SetRoundInfo(WM->GetCurrentRoundNumber());
		}
	}

	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void ARuptureHUD::HideDeathScreen()
{
	if (DeathScreenWidget)
	{
		DeathScreenWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void ARuptureHUD::OnAllRoundsCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("HUD: 10 rounds completos — vitória!"));
	ShowVictoryScreen();
}

void ARuptureHUD::ShowVictoryScreen()
{
	if (!VictoryScreenWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("HUD: VictoryScreenWidget é null. Sete VictoryScreenClass no BP_RuptureHUD."));
		return;
	}

	if (!VictoryScreenWidget->IsInViewport())
	{
		VictoryScreenWidget->AddToViewport(20);
	}

	VictoryScreenWidget->ShowVictory();

	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void ARuptureHUD::HideVictoryScreen()
{
	if (VictoryScreenWidget)
	{
		VictoryScreenWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

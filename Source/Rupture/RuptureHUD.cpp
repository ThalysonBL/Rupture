#include "RuptureHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RuptureDeathScreenWidget.h"
#include "RupturePlayerCharacter.h"
#include "RupturePlayerHUDWidget.h"
#include "RuptureWaveManager.h"
#include "RuptureWeaponBase.h"


void ARuptureHUD::BeginPlay() {
  Super::BeginPlay();

  if (PlayerHUDClass) {
    PlayerHUDWidget =
        CreateWidget<URupturePlayerHUDWidget>(GetWorld(), PlayerHUDClass);

    if (PlayerHUDWidget) {
      PlayerHUDWidget->AddToViewport();

      ARupturePlayerCharacter *PlayerChar = Cast<ARupturePlayerCharacter>(
          UGameplayStatics::GetPlayerCharacter(this, 0));
      if (PlayerChar) {
        UHealthComponent *HealthComp =
            PlayerChar->FindComponentByClass<UHealthComponent>();
        if (HealthComp) {
          // Assina o evento para escutar as alterações de vida
          HealthComp->OnHealthChanged.AddDynamic(
              this, &ARuptureHUD::OnPlayerHealthChanged);
          HealthComp->OnDeath.AddDynamic(this, &ARuptureHUD::OnPlayerDied);
          // Atualiza a barra inicial
          PlayerHUDWidget->UpdateHealth(100.f, 100.f);
        }

        PlayerChar->OnWeaponEquipped.AddDynamic(
            this, &ARuptureHUD::OnWeaponEquippedCallback);

        if (DeathScreenClass) {
          DeathScreenWidget = CreateWidget<URuptureDeathScreenWidget>(
            GetWorld(), DeathScreenClass);
        }
      }
    }
  }
}

void ARuptureHUD::OnPlayerHealthChanged(float NewHealth, float MaxHealth) {
  if (PlayerHUDWidget) {
    PlayerHUDWidget->UpdateHealth(NewHealth, MaxHealth);
  }
}

void ARuptureHUD::OnWeaponEquippedCallback(ARuptureWeaponBase *NewWeapon) {
  if (NewWeapon) {
    // 1. O HUD se inscreve no evento de tiros/reload da arma
    NewWeapon->OnAmmoChanged.AddDynamic(this,
                                        &ARuptureHUD::OnAmmoChangedCallback);

    // 2. Força a tela a mostrar a munição inicial logo no instante do Spawn
    if (PlayerHUDWidget) {
      // Assumindo que sua arma tenha esses métodos Getters. Se não tiver,
      // crie-os!
      PlayerHUDWidget->UpdateAmmo(NewWeapon->GetCurrentAmmo(),
                                  NewWeapon->GetReserveAmmo());
    }
  }
}

void ARuptureHUD::OnAmmoChangedCallback(int32 CurrentAmmo, int32 ReserveAmmo) {
  // Quando a arma grita "Atirei!", o HUD repassa os novos números para a tela
  if (PlayerHUDWidget) {
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

	// Mostra round atual
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
#include "RuptureHUD.h"
#include "RupturePlayerHUDWidget.h"
#include "RupturePlayerCharacter.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RuptureWeaponBase.h"
#include "RuptureWeaponBase.h"
#include "RupturePlayerHUDWidget.h"

void ARuptureHUD::BeginPlay()
{
	Super::BeginPlay();
    
	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<URupturePlayerHUDWidget>(GetWorld(), PlayerHUDClass);

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();

			ARupturePlayerCharacter* PlayerChar = Cast<ARupturePlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
			if (PlayerChar)
			{
				UHealthComponent* HealthComp = PlayerChar->FindComponentByClass<UHealthComponent>();
				if (HealthComp)
				{
					// Assina o evento para escutar as alterações de vida
					HealthComp->OnHealthChanged.AddDynamic(this, &ARuptureHUD::OnPlayerHealthChanged);
                  
					// Atualiza a barra inicial
					PlayerHUDWidget->UpdateHealth(100.f, 100.f); 
				}
				
				PlayerChar->OnWeaponEquipped.AddDynamic(this, &ARuptureHUD::OnWeaponEquippedCallback);
			}
		}
	}
}

void ARuptureHUD::OnPlayerHealthChanged(float NewHealth)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->UpdateHealth(NewHealth, 100.f); 
	}
}


void ARuptureHUD::OnWeaponEquippedCallback(ARuptureWeaponBase* NewWeapon)
{
	if (NewWeapon)
	{
		// 1. O HUD se inscreve no evento de tiros/reload da arma
		NewWeapon->OnAmmoChanged.AddDynamic(this, &ARuptureHUD::OnAmmoChangedCallback);

		// 2. Força a tela a mostrar a munição inicial logo no instante do Spawn
		if (PlayerHUDWidget)
		{
			// Assumindo que sua arma tenha esses métodos Getters. Se não tiver, crie-os!
			PlayerHUDWidget->UpdateAmmo(NewWeapon->GetCurrentAmmo(), NewWeapon->GetReserveAmmo());
		}
	}
}

void ARuptureHUD::OnAmmoChangedCallback(int32 CurrentAmmo, int32 ReserveAmmo)
{
	// Quando a arma grita "Atirei!", o HUD repassa os novos números para a tela
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->UpdateAmmo(CurrentAmmo, ReserveAmmo);
	}
}
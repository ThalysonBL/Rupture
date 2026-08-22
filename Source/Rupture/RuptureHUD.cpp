#include "RuptureHUD.h"
#include "RupturePlayerHUDWidget.h"
#include "RupturePlayerCharacter.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"

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
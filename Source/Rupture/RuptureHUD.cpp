#include "RuptureHUD.h"
#include "Blueprint/UserWidget.h"

void ARuptureHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlayerHUDClass)
	{
		// Instancia o Widget usando o jogador atual como dono
		PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
}
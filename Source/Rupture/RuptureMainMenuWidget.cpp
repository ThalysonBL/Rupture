#include "RuptureMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void URuptureMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (btn_StartGame)
	{
		btn_StartGame->OnClicked.AddDynamic(this, &URuptureMainMenuWidget::OnStartGameClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenu: btn_StartGame não encontrado. Verifique o BindWidget no WBP."));
	}
}

void URuptureMainMenuWidget::OnStartGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("MainMenu: Iniciar jogo -> %s"), *GameLevelName.ToString());
	UGameplayStatics::OpenLevel(this, GameLevelName);
}

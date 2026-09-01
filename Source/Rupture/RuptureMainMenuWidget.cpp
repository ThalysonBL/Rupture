#include "RuptureMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "RuptureGameInstance.h"

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

	if (URuptureGameInstance* GameInstance = Cast<URuptureGameInstance>(GetGameInstance()))
	{
		GameInstance->OpenLevelWithLoadingScreen(
			GameLevelName,
			FText::FromString(TEXT("Carregando missão..."))
		);
		return;
	}

	UGameplayStatics::OpenLevel(this, GameLevelName);
}

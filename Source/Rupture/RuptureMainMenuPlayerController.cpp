#include "RuptureMainMenuPlayerController.h"
#include "RuptureMainMenuWidget.h"

void ARuptureMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());

	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuPC: MainMenuWidgetClass não setada. Defina no BP_MainMenuGameMode / Class Defaults."));
		return;
	}

	MainMenuWidget = CreateWidget<URuptureMainMenuWidget>(this, MainMenuWidgetClass);
	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport(0);
		UE_LOG(LogTemp, Warning, TEXT("MainMenuPC: menu adicionado ao viewport."));
	}
}

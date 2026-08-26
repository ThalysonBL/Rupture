#include "RuptureMainMenuGameMode.h"
#include "RuptureMainMenuPlayerController.h"

ARuptureMainMenuGameMode::ARuptureMainMenuGameMode()
{
	PlayerControllerClass = ARuptureMainMenuPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
}

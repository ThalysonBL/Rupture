#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RuptureMainMenuGameMode.generated.h"

/**
 * GameMode do menu inicial: sem pawn de combate, só UI.
 * Use via BP_MainMenuGameMode ou diretamente no World Settings de L_MainMenu.
 */
UCLASS()
class RUPTURE_API ARuptureMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARuptureMainMenuGameMode();
};

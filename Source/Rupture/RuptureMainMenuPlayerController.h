#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RuptureMainMenuPlayerController.generated.h"

UCLASS()
class RUPTURE_API ARuptureMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class URuptureMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	class URuptureMainMenuWidget* MainMenuWidget;
};
